const tsp = `
(BLOCK
  :attribute
  My text

  @section1
  :a1
  @section2
  :a2
)`;


function tinysplitLine (state, line) {
  state       ??= {};
  state.push    = null;
  state.pop     = null;
  state.stack ??= [];

  state.line    = line;
  state.trimmed = line.trim();
  state.sigil   = state.trimmed.charAt(0);
  state.payload = state.trimmed.slice(1);

  let pop_from = state.stack.length;

  switch (state.sigil) {
    case "":
      state.sigil = null;
      return state;

    case "(":
    case ":":
      state.push = state.trimmed;
      break;

    case ")":
      for (let p=state.stack.length-1; p >= 0; p--) {
        if (state.stack[p][0] == "(") {
          pop_from = p;
          break;
        }
      }
      break;

    case "@":
      for (let p=state.stack.length-1; p >= 0; p--) {
        switch (state.stack[p][0]) {
          default:
            continue; // don't pop anything
          case "@":
            pop_from = p;
          case "(":
        }
        break;
      }

      if (state.trimmed.length > 1) {
        state.push = state.trimmed;
      }
      break;
  }

  if (pop_from != state.stack.length) {
    state.pop = state.stack.slice(pop_from);
  }

  if (state.push) {
    state.push            = state.push;
    state.stack.length    = pop_from + 1;
    state.stack[pop_from] = state.push;
  } else {
    state.stack.length = pop_from;
  }

  return state;
}

console.log(
  tsp.split("\n").reduce(
    (s, l) => (console.log(s), tinysplitLine(s,l)), {}
  ),
);
