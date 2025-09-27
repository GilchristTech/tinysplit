# TinySplit (Teaspoon)

TinySplit, or Teaspoon for short\*, is a
document-like text miniformat which seeks to
reframe data around how it is handled by
applications. Most formats focus on being
converted into an intermediate data structure to
be parsed by applications, but TinySplit is
designed first with streaming and iteration in
mind. 

\* &mdash; A TinySplit file has the `.tsp`
    extension. Hence, Teaspoon!

Finding inspiration in the philosophy of the Forth
school of programming languages, TinySplit is
intended to be implemented by the individual
developer, rather than imported as a module. This
is not to say that a Teaspoon module shouldn't
exist, but that ease of implementation is a
priority.  Ideally, TinySplit can be written
in most programming languages in under 100 lines
using a couple of loops and switch statements, and
is intuitive enough to do from memory. If an
implementation does not provide the ideal usage
for you, making your own won't take very long!

```
(my-block
    Here is some text about my block.

    @section-1
    :attribute-a
    Some text about section-1, which
    may relate to attribute-a and my-block.

    @section-2
    :attribute-b
    This text, however, is scoped to my-block,
    section-2, and attribute-b.
)
```

In structured file formats, interchange formats,
and protocols, the application often does not use
the data in its immediately-parsed form. For
instance, JSON values or fields get reshaped into
the data structures of the language, or they
direct routines in the host application. Because
of this, TinySplit is more focused around
streaming and iteration of text, and does not
prescribe a particular shape the data should be
formed into within a program. 

# The Language

In TinySplit, lines are handled one at a time, in
order. After a line is trimmed of whitespace,
the first character is called the sigil, and
everything after it is the payload. The sigil is
used to determine nesting and scope of this line
and those at lower scopes.

TinySplit has four different sigils:

- Blocks, `(' and `)`:
    Blocks are opened and closed by
    parenthesis. Sections, attributes, and text
    are scoped inside blocks.

- Sections, `@`:
    Sections are declared with an `@` sign, and
    are scoped within their parent block and its
    attributes. When a section is encountered, the
    previous section under the current block (or
    root) is removed from the stack. 

- Attributes `:`:
    Attributes are added to the stack as-is, but
    define scope for lower blocks, sections, and
    text.

If a line lacks one of the above sigils, it is
treated as plain text, and it does not modify the
stack. Its scope is any parent blocks and
sections, as well as their attributes.
Implementations should have access to this scope
information when processing text so that the
application can determine how to use it, and to
allow an application to be written to allow custom
sigils or make text lines modify the stack.

# TinySplit Algorithm

A Teaspoon implementation should iterate through
lines of input text. The only required state
between these iterations is a stack, which tells
the application the scope of the line currently
being processed.

When TinySplit processes a line, it uses this algorithm:

* Trim the line's whitespace. Let the first
  character be the sigil, and let all characters
  after be the payload.

* Based on the sigil, perform one of the
  following: 

    * On `:`
      and `(`: Push the trimmed line onto the
      stack.

    * On `)`:
      Pop items from the stack until an
      item with a `(` sigil is popped, or until
      the stack is emptied.

    * On `@`:
      - Peek at items from the stop of a stack
        until one with a `@` sigil is found,
        but terminate the search if a `(`
        sigil is found. 

      - If the `@` item was found, pop all
        searched items from the stack.

      - If the payload's length is one or more,
        push the trimmed line onto the stack.
        This allows for an `@` with no payload to
        terminate a section.

    * If the sigil is none of these characters, it
      is a text line. Perform no stack operations.

* Either perform callback functions, or
  return values, or mutate memory based on:
  - Whether a value was pushed onto the stack.
  - Which items were popped from the stack.
  - The trimmed text, sigil, and payload, or
    unmodified text line.

At its lowest level, with each line iteration, an
implementation can expose the line, trimmed line,
sigil, payload, stack, pushed line, and popped
lines. A higher-level implementation may expose
these through callback functions. Another
implementation may attempt to organize this into a
data structure. These are considered
implementation details, however.

Some implementations may wish to pop all items from
the stack after iterating over each line,
particularly if an application needs to respond to
items being popped from the stack.

## Example Implementations

The repository contains a series of example
implementations in various programming languages
in the `examples/` directory.
