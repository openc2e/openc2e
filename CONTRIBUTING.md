# Contributor Guidelines

If you have any questions about openc2e development or would like to help, come find us on [the #openc2e channel on the Caos Coding Cave Discord](https://discord.gg/rWFC3b3)!

## Code licensing

If you make any contributions to openc2e after January 9th, 2025, you are agreeing that any code you have contributed will be licensed under the GNU LGPL version 2.1 (or any later version).

## Code style and formatting

In most cases, `clang-format` can and should be used to automatically reformat code and solve most formatting issues.

- To run clang-format on all staged files:
  ```sh
  ./script/lint.sh
  ```

- To run clang-format on all files changed since a certain revision, or that are different from another branch, pass the ref name as the first argument to `lint.sh`. For instance, to check all files different from the current main branch:
  ```sh
  ./script/lint.sh main
  ```



