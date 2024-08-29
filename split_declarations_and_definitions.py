import re
from pathlib import Path


def parse_function(content: str):
    pattern = r"^(?P<head>.*?)void init(?P<module>\S+)\(py::module\s?&\s?m\) {(?P<body>.*)}(?P<tail>.*?)$"
    if matched := re.match(pattern, content, re.DOTALL):
        return matched.groupdict()


def split_parts(content: str):
    parts = []
    lines = content.splitlines()
    start_index = 0
    is_annotation = False
    for i, line in enumerate(lines):
        if line.startswith("/*"):
            is_annotation = True
        if line.endswith("*/"):
            is_annotation = False
        if (
            not line.strip()
            and not is_annotation
            and (i > start_index and lines[i - 1].endswith((";", "*/")))
            and (i + 1 >= len(lines) or not lines[i + 1].startswith(" "))
        ):
            parts.append("\n".join(lines[start_index:i]))
            start_index = i + 1
    parts.append("\n".join(lines[start_index:]))
    parts = [part.strip() for part in parts if part.strip()]
    return parts


def is_pybind_enum(code: str):
    return code.startswith("py::enum_")


def is_pybind_class(code: str):
    return code.startswith("py::class_")


def is_pybind_module(code: str):
    return code.startswith("py::module")


def parse_class_signature(code: str):
    pattern = r"py::class_<(?P<type>.+?)>(?:\s+(?P<variable>\S+))?\(\s*(?P<module>\S+),\s*\"(?P<name>\S+)\""
    if matched := re.match(pattern, code, re.DOTALL):
        return matched.groupdict()


def split_pybind_class(code: str):
    lines = code.splitlines()
    definition_index = len(lines)
    for i, line in enumerate(lines):
        if line.lstrip().startswith((".def", "/*")):
            definition_index = i
            break
    declaration_part = "\n".join(lines[:definition_index]).strip()
    definition_part = "\n".join(lines[definition_index:]).rstrip()
    if not declaration_part.endswith(";"):
        declaration_part += ";"

    if not (result := parse_class_signature(declaration_part)):
        raise ValueError(f"Failed to match class signature: {declaration_part}")
    type, variable, module, name = result.values()

    def variable_definition(variable_name: str):
        return f'auto {variable_name} = static_cast<py::class_<{type}>>({module}.attr("{name}"));'

    if not definition_part.strip():
        definition_part = ""
        # if there is no definition part but a variable is defined,
        # we also need to define the variable in the definition part
        if variable:
            definition_part = variable_definition(variable)
    else:
        variable_name = variable or name.lower()
        definition_part = (
            variable_definition(variable_name)
            + "\n"
            + variable_name
            + "\n"
            + definition_part
        )

    return declaration_part, definition_part


def split_pybind_module(code: str):
    pattern = r"py::module\s+(?P<variable>\S+)\s*=\s*(?P<module>\S+).def_submodule\(\"(?P<name>\S+)\"\)"
    if not (matched := re.match(pattern, code, re.DOTALL)):
        raise ValueError(f"Failed to match module definition: {code}")
    variable, module, name = matched.groups()
    declaration_part = code
    definition_part = (
        f'auto {variable} = static_cast<py::module>({module}.attr("{name}"));'
    )
    return declaration_part, definition_part


def split_head_annotation(part: str):
    lines = part.splitlines()
    annotation_index = 0
    is_annotation = False
    for line in lines:
        if line.startswith("/*"):
            is_annotation = True
        if is_annotation or line.startswith("//"):
            annotation_index += 1
        else:
            break
        if line.endswith("*/"):
            is_annotation = False
    annotation = "\n".join(lines[:annotation_index])
    code = "\n".join(lines[annotation_index:])
    return annotation, code


def process_file(file: Path, output_dir: Path):
    with open(file, "r") as f:
        content = f.read()
    if not (result := parse_function(content)):
        raise ValueError(f"Failed to process {file}")

    head, module, body, tail = result.values()

    parts = split_parts(body)
    declaration_parts = []
    definition_parts = []
    for part in parts:
        # in the case that there is annotation before the code
        annotation, code = split_head_annotation(part)
        if is_pybind_enum(code):
            declaration_parts.append(part)
        elif is_pybind_module(code):
            declaration_part, definition_part = split_pybind_module(code)
            if annotation:
                declaration_part = annotation + "\n" + declaration_part
            declaration_parts.append(declaration_part)
            definition_parts.append(definition_part)
        elif is_pybind_class(code):
            declaration_part, definition_part = split_pybind_class(code)
            if annotation:
                declaration_part = annotation + "\n" + declaration_part
            declaration_parts.append(declaration_part)
            if definition_part:
                definition_parts.append(definition_part)
        else:
            definition_parts.append(part)

    result_template = """{head}void init{module}Declarations(py::module &m) {{
{declaration_parts}
}}

void init{module}Definitions(py::module &m) {{
{definition_parts}
}}{tail}
"""
    result = result_template.format(
        head=head,
        module=module,
        declaration_parts="\n\n".join(declaration_parts),
        definition_parts="\n\n".join(definition_parts),
        tail=tail,
    )
    with open(output_dir / file.name, "w") as f:
        f.write(result)


if __name__ == "__main__":
    input_dir = Path("src/skia")
    output_dir = input_dir
    for file in input_dir.glob("*.cpp"):
        if file.name in ["main.cpp", "utils.cpp"]:
            continue
        process_file(file, output_dir)
