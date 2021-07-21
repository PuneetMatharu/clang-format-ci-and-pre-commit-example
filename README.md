# clang-format-ci-example

![clang-format workflow](https://github.com/PuneetMatharu/clang-format-ci-example/actions/workflows/clang-format.yml/badge.svg)

A simple project that demonstrates how to use `clang-format` in a CI workflow.

## CI workflow

The `.clang-format` file provides the format style specification to `clang-format`
and the `.github/workflows/clang-format.yml` GitHub Action applies the formatting.

## Pre-commit hook
### Ubuntu
First, make sure you have `pip` available. On Ubuntu, run
```
sudo apt-get install python3-pip
```
Once you have `pip`, you simply need to download [`pre-commit`](https://pre-commit.com/)
and install the Git hooks. To do this, run the following:
```bash
pip install pre-commit
pre-commit install
```
### MacOS
**Option 1: Use Homebrew**

If you have [Homebrew](https://brew.sh/) installed, all you need to do is run
```bash
brew install pre-commit
pre-commit install
```
**Option 2: Use MacPorts**

Alternatively, if you have [MacPorts](https://ports.macports.org/) installed, you just need to run
```bash
sudo port install pre-commit
pre-commit install
```

**Option 3: Use pip**

If you do not wish to use either [Homebrew](https://brew.sh/) or
[MacPorts](https://ports.macports.org/), you can install and use `pip` by running
```bash
curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
python3 get-pip.py
rm -f get-pip.py
```
That's all it takes. Typically, a pre-commit configuration file must also be
provided within your project, but one has already been provided here. When you
run `git commit`, the hooks specified in the `.pre-commit-config.yaml` file will
be executed.
