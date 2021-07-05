# clang-format-ci-example

![clang-format workflow](https://github.com/PuneetMatharu/clang-format-ci-example/actions/workflows/clang-format.yml/badge.svg)

A simple project that demonstrates how to use `clang-format` in a CI workflow.

## CI workflow

The `.clang-format` file provides the format style specification to `clang-format`
and the `.github/workflows/clang-format.yml` GitHub Action applies the formatting.

## Pre-commit hook

First, install the [`pre-commit` package manager](https://pre-commit.com/). You
can do this with `pip` using the following command:
```bash
pip install pre-commit
```
Next, install the git hook scripts with
```bash
pre-commit install
```
That's all it takes. Typically, a pre-commit configuration file must also be
provided within your project, but one has already been provided here. When you
run `git commit`, the hooks specified in the `.pre-commit-config.yaml` file will
be executed.
