from pprint import pprint

tsp = """
(BLOCK
    :a1
    @section1
    :a2
    Section 1 text
    @section2
    :a3
    Section 2 text
)
"""

def tinysplitIter (lines):
    stack = []
    state = { "stack": stack }

    for line in lines:
        state["line"]    = line
        state["trimmed"] = trimmed = line.strip()
        state["sigil"]   = sigil   = trimmed[:1]
        state["payload"] = trimmed[1:]

        state["push"] = None
        state["pop"]  = None

        pop_from = len(stack)

        match sigil:
            case "(" | ":":
                state["push"] = trimmed

            case ")":
                for o, item in enumerate(reversed(stack)):
                    if item[0] == "(":
                        pop_from = len(stack) - o - 1
                        break

            case "@":
                state["push"] = trimmed
                for o, item in enumerate(reversed(stack)):
                    match item[0]:
                        case "@": pop_from = len(stack) - o - 1
                        case "(": pass
                        case _:   continue
                    break

        if pop_from < len(stack):
            state["pop"] = stack[pop_from:]
            del stack[pop_from:]

        if state["push"]:
            stack.append(state["push"])

        yield state


state = {}
for state in tinysplitIter(tsp.split("\n")):
    pprint(state)
    print()
