# Contributor guidelines

This document is meant to help developers understand how to make contributions to the openc2e project.

## Code style conventions

### C/C++

C/C++ files in openc2e use `clang-format` for formatting.

Before creating a pull request, format your code using `clang-format` 10.x.

Use the lint script to automatically reformat your code to match openc2e's conventions:

```bash
./scripts/lint.py
```

## Commit hygiene

openc2e merges PRs as merge commits, which means that all of the commits in your PR become part of the commit history on openc2e's main branch.

Because of this, you should ensure that all of your commits are logical, self-contained units with descriptive commit messages.

If you have temporary or WIP commits, squash them into a clean change history before opening a PR.
