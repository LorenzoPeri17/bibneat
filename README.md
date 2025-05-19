# bibneat

Making your BibTeX files nice and neat (pun absolutely intended)!

## What is bibneat?

`bibneat` is your friendly command-line companion for wrangling, cleaning, and supercharging your `.bib` files. Whether you’re a LaTeX enthusiast, a citation perfectionist, or just tired of messy bibliographies giving arcane warnings, bibneat is here to help you:

- **Format your .bib files beautifully and consistently**
- **Merge multiple bibfiles** with ease (no more duplicate headaches)
- **Shorten and clean up entries** by filtering out fields not used by BibTeX
- **Handle Unicode like a pro**:
  - Canonical (NFC) and compatibility-optimized (NFKC) Unicode normalization
  - Convert Unicode characters (accents, symbols, etc.) to their equivalent LaTeX encoding
- **Connect to arXiv.org and doi.org APIs** to:
  - Check and validate bibliography entries
  - Fill in missing fields automatically
  - Detect if an arXiv entry has been published and (optionally) replace it with the published version

## Bibneat’s Pinkie Promises

- If your `.bib` file compiled before, it will compile after bibneat touches it. Guaranteed.
- Your bibliography keys are sacred. bibneat will **never** change them, so every `\cite{}` in your documents will keep working, even if entries are modified or replaced.

## Meet bibgrab: The Little Sibling

Need to quickly turn an arXiv ID, arXiv URL, or DOI into a ready-to-cite BibTeX entry? `bibgrab` is your speedy sidekick! Just feed it an identifier, and it’ll fetch and format the entry for you—no fuss, no muss.

## Installation

`bibneat` depends on:

- The C++ standard library
- [libcurl](https://curl.se/libcurl/) (for web requests)
- [ICU4C](https://icu.unicode.org/) (the official source of Unicode and normalization magic)

We use `cmake` and [`vcpkg`](https://learn.microsoft.com/en-us/vcpkg/)  to automagically manage dependencies. If you have both set up, just type

```sh
cmake --preset=release
cmake --build build
```

and you’re off to the races!

For more details, or if you prefer managing dependencies yourself and use `make`, see [`INSTALL.md`](INSTALL.md).

## Library

`bibneat` is not just a command-line tool—it also provides a C++ shared library (`libbibneat`) for building your own BibTeX-powered tools and utilities. See the [docs/tex/libbibneat.tex](docs/tex/libbibneat.pdf) for an API overview.

## Why bibneat?

Because life’s too short for ugly bibliographies. ✨

---

For more details, usage instructions, and advanced options, check out `bibneat --help`, `bibgrab --help`, or the pdf [docs](docs/tex/bibneat.pdf).

---

Thank you to [arXiv](https://info.arxiv.org/help/api/index.html) (and to the [DOI](https://www.doi.org/) foundation) for use of its open access interoperability :)

Happy (Bib)TeXing!
