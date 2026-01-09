#! /usr/bin/env python

import inspect
from pathlib import Path
import persalys
import re

# find all instantiable classes
instantiables = []
for mod in [persalys]:
    for name, obj in inspect.getmembers(mod):
        if inspect.isclass(obj):
            cn = obj.__name__
            if "_" in cn:
                continue
            try:
                instance = obj()
                instantiables.append(obj)
            except Exception:
                pass

# find missing docstrings
count_class = 0
count_class_undoc = 0
count_methods = 0
count_methods_undoc = 0
for class_ in instantiables:
    count_class += 1
    if class_.__doc__ is None:
        print(f"{class_.__name__} class")
        count_class_undoc += 1
    for attr_name in dir(class_):
        if attr_name[0] == "_":
            continue
        class_attr = getattr(class_, attr_name)
        if callable(class_attr):
            count_methods += 1
            if class_attr.__doc__ is None:
                print(f"{class_.__name__}.{attr_name} method")
                count_methods_undoc += 1

# find all static functions
for name, mod in inspect.getmembers(persalys):
    if inspect.ismodule(mod):
        modn = mod.__name__
        if not modn.startswith("persalys") or modn == "persalys":
            continue
        if "_" in modn:
            continue

        for attr_name in dir(mod):
            if "_" in attr_name:
                continue
            obj = getattr(mod, attr_name)

            if not inspect.isclass(obj) and callable(obj):
                symboln = f"{modn.split('.')[1]}.{attr_name}"
                count_methods += 1
                if obj.__doc__ is None:
                    count_methods_undoc += 1
                    print(f"{symboln} method")


print(
    f"-- undocumented classes: {count_class_undoc} ({100.0 * count_class_undoc / count_class:.2f}%) --"
)
print(
    f"-- undocumented methods: {count_methods_undoc} ({100.0 * count_methods_undoc / count_methods:.2f}%) --"
)
if count_class_undoc + count_methods_undoc > 150:
    raise ValueError(
        f"too many undocumented class/methods ({count_class_undoc + count_methods_undoc})"
    )

# count extra docstrings methods
count_methods_extra = 0
for swig_file in Path(__file__).parents[1].joinpath("src").glob("*_doc.i"):
    with open(swig_file) as f:
        for line in f.read().splitlines():
            match = re.search(r'%feature\("docstring"\) PERSALYS::([\w]*)::([\w]*)', line)
            if match is not None:
                cn = match.group(1)
                mn = match.group(2)
                try:
                    cls = getattr(persalys, cn)
                    obj = cls()
                    if not hasattr(obj, mn):
                        print(f"extra {cn}::{mn} method")
                        count_methods_extra += 1
                except Exception:
                    pass
print(f"-- extra method docstrings: {count_methods_extra}")
if count_methods_extra > 10:
    raise ValueError(f"too many extra method docstrings ({count_methods_extra})")
