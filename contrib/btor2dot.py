#!/usr/bin/env python3

import sys

def shape(kind):
    if kind == "root":
        return "none"
    elif "const" in kind or kind in ["array", "var", "param"]:
        return "box"
    else:
        return "oval"

def arrow(from_id, to_id, label = ""):
    arrowshape="normal"
    if to_id < 0:
        to_id = -to_id
        arrowshape="dot"
    print("{} -> {} [arrowhead=\"{}\", taillabel=\"{}\"]".format(
          from_id, to_id, arrowshape, label))

def node(id, shape, label, style = "", fillcolor = ""):
    s = "{} [shape={}, label=\"{}\"".format(id, shape, label)
    if style != "":
        s += ", style={}".format(style)
    if fillcolor != "":
        s += ", fillcolor={}".format(fillcolor)
    s += "]"
    print(s)

if __name__ == "__main__":

    print("digraph G {")

    array_nodes = ["array", "acond", "write", "lambda"]
    leaf_nodes = ["array", "var", "param", "const", "consth", "constd"]
    param_nodes = {}
    roots = []

    try:
        for line in sys.stdin:
            t = line.split()
            id = int(t[0])
            kind = t[1]

            if kind == "path":
                nodes = t[2:]
                nodes.reverse()
                for i in range(1, len(nodes)):
                    print("{} -> {} [fontcolor=\"blue\", color=\"blue\"," \
                          "label=\"L{}\"]".format(nodes[i - 1], nodes[i], id))
                continue


            assert(id > 0)

            # start index of children
            if not kind in leaf_nodes:
                offset = 3
                if kind in array_nodes:
                    offset += 1

                children = [int(i) for i in t[offset:]]
            else:
                children = []

            # set default node label, style
            label = "{}: {}".format(id, kind)
            style = ""
            fillcolor = ""

            # set node specific stuff
            if kind == "root":
                label = ""
                roots.append(id)
            elif kind == "slice":
                upper = children[1]
                lower = children[2]
                children = [children[0]]
                label = "{}\\n{} {}".format(label, upper, lower)
            elif kind in array_nodes:
                style = "filled"
                fillcolor = "lightblue"
            elif kind == "apply":
                style = "filled"
                fillcolor = "lightyellow"
            elif kind == "param":
                param_nodes[id] = True

            # check if node has parameterized children (stop at lambda)
            if kind != "lambda":
                for c in children:
                    if abs(c) in param_nodes:
                        param_nodes[id] = True
                        break

            # set style for parameterized nodes
            if id in param_nodes:
                style = "dotted"
                fillcolor = ""

            node(id, shape(kind), label, style, fillcolor)

            # draw edges with labels if arity > 1
            i = 1
            for c in children:
                label = ""
                if len(children) > 1:
                    label = str(i)
                arrow(id, c, label)
                i += 1

    except IOError as err:
        sys.exit(err)

#    # set roots as same rank
#    print("{{rank=same; {}}}".format(" ".join([str(r) for r in roots])))

    print("}")
