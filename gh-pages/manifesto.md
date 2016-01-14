---
layout: default
title: Neon Manifesto
---

Programming languages today are full of features, and there is a trend toward brevity.
Brevity is highly expressive, for an expert who understands the notation.
However, for a new learner or a casual user, brevity promotes unreadable code.

## Principles of Design

* Prefer keywords over punctuation
* Good error messages
* Avoid implicit behaviour
* Explicit declarations
* Single meaning per keyword/operator
* Principle of least surprise
* Easy lookup

### Prefer keywords over punctuation

Punctuation is important, but it should be used sparingly.

### Good error messages

Error messages should explain what is wrong, with suggestions of ways to correct the problem.

### Avoid implicit behaviour

There will be no hidden, implicit, or automatic calls made to code that is not expressly visible in the source.

### Explicit declarations

Everything is declared, and identifiers from modules are always qualified.

### Single meaning per keyword/operator

To the greatest reasonable extent, keywords and operators only have one meaning.

### Principle of least surprise

This one is hard to quantify, but it's good.

### Easy lookup

By using keywords instead of punctuation, and by using explicit declarations, it is easy to look up unknown things in the documentation.
