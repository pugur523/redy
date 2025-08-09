#!/usr/bin/env python3

# Copyright 2025 pugur
# This source code is licensed under the Apache License, Version 2.0
# which can be found in the LICENSE file.

import tomllib
import sys
from typing import Any

from build_util import project_config_file


def load_project_metadata(file_path: str) -> dict[str, Any]:
    try:
        with open(file_path, "rb") as f:
            data = tomllib.load(f)
            return data["project"]
    except FileNotFoundError:
        print(f"error: config file not found: {file_path}", file=sys.stderr)
        sys.exit(1)
    except KeyError:
        print(f"error: missing [project] section in {file_path}", file=sys.stderr)
        sys.exit(2)
    except tomllib.TOMLDecodeError as e:
        print(f"error: failed to parse toml: {e}", file=sys.stderr)
        sys.exit(3)


def main() -> int:
    prj = load_project_metadata(project_config_file)

    print(prj.get("name", "<no name>"))
    print(prj.get("main_executable_name", "<no executable name>"))
    print(prj.get("version", "<no version>"))
    print(prj.get("description", "<no description>"))
    print(prj.get("homepage", "<no homepage>"))
    print(prj.get("c_version", "<no c version>"))
    print(prj.get("cxx_version", "<no c++ version>"))
    print(prj.get("author", "<no author>"))
    print(prj.get("author_email", "<no author email>"))

    return 0


if __name__ == "__main__":
    sys.exit(main())
