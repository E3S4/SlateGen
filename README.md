# SlateGen

**SlateGen** is a lightweight, no-nonsense static site generator written in modern C++.  
It turns Markdown into clean HTML — and because symmetry is sexy, it can flip HTML back into Markdown too.

Think of it as a minimalist’s weapon for creating portfolio sites, docs, or blogs without dragging in bloated frameworks.  
If you like fast binaries and zero dependencies, you’re in the right place.

---

## Features

- Converts **Markdown → HTML** and **HTML → Markdown**
- Simple CLI. No nonsense, no weird config rituals
- Auto-copies static files like CSS, JS, and images
- Outputs ready-to-serve HTML
- Portable C++17 code — compile it anywhere
- Open source. Obviously.

---

### Build it

```
mkdir build
cd build
cmake ..
cmake --build .
```

This will drop an executable named `slategen` inside the build directory.

---

### Convert Markdown → HTML

```
./slategen --to-html ./src ./site

```


SlateGen scans your `src` folder for `.md` files, converts them to `.html`,  
and copies all non-Markdown files (like `style.css`) straight over.

Output lands neatly in `./site`.

---

### Convert HTML → Markdown

```
./slategen --to-md ./site ./markdown

```


Because maybe you wrote your blog in raw HTML like it’s 2003 and now regret it.  
This flips it back into Markdown that actually looks human-editable again.

---

### Folder example

src/
├── index.md
├── about.md
└── assets/
└── style.css


After running SlateGen:
site/
├── index.html
├── about.html
└── assets/
└── style.css


Simple, predictable, no magic.

---

## Why “SlateGen”?

Because it’s your **clean slate** for generating sites.  
Nothing fancy — just you, your words, and a compiler that doesn’t whine.

---

## License

GNU GPL License.  
Do whatever you want, but don’t pretend it's your own , it's our code .



