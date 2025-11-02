#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <regex>
#include <map>

namespace fs = std::filesystem;

// Read a file fully into a string
std::string read_file(const fs::path& path) {
    std::ifstream f(path);
    std::stringstream buffer;
    buffer << f.rdbuf();
    return buffer.str();
}

// Write content to a file (auto creates directories)
void write_file(const fs::path& path, const std::string& content) {
    fs::create_directories(path.parent_path());
    std::ofstream f(path);
    f << content;
}

// Very minimal Markdown → HTML converter
std::string markdown_to_html(const std::string& markdown) {
    std::string html = markdown;
    html = std::regex_replace(html, std::regex(R"(\*\*(.*?)\*\*)"), "<b>$1</b>");
    html = std::regex_replace(html, std::regex(R"(\*(.*?)\*)"), "<i>$1</i>");
    html = std::regex_replace(html, std::regex(R"(^# (.*)$)", std::regex_constants::multiline), "<h1>$1</h1>");
    html = std::regex_replace(html, std::regex(R"(^## (.*)$)", std::regex_constants::multiline), "<h2>$1</h2>");
    html = std::regex_replace(html, std::regex(R"(^### (.*)$)", std::regex_constants::multiline), "<h3>$1</h3>");
    html = std::regex_replace(html, std::regex(R"(\[(.*?)\]\((.*?)\))"), "<a href=\"$2\">$1</a>");
    html = std::regex_replace(html, std::regex(R"(\n\n)"), "<br><br>");
    return html;
}

// Replace {{placeholders}} with actual values in the HTML template
std::string apply_template(const std::string& tmpl, const std::map<std::string, std::string>& vars) {
    std::string out = tmpl;
    for (const auto& [key, val] : vars) {
        out = std::regex_replace(out, std::regex("\\{\\{" + key + "\\}\\}"), val);
    }
    return out;
}

// Walk through the source directory, converting .md → .html, and copying other files
void process_directory(const fs::path& src, const fs::path& dst, const std::string& tmpl, const std::string& style_path) {
    for (const auto& entry : fs::recursive_directory_iterator(src)) {
        if (entry.is_regular_file()) {
            auto rel = fs::relative(entry.path(), src);
            auto out_path = dst / rel;

            // If markdown file → convert and write as .html
            if (entry.path().extension() == ".md") {
                std::string md = read_file(entry.path());
                std::string html_content = markdown_to_html(md);

                std::map<std::string, std::string> vars = {
                    {"title", entry.path().stem().string()},
                    {"style", style_path},
                    {"content", html_content}
                };

                std::string html = apply_template(tmpl, vars);
                out_path.replace_extension(".html");
                write_file(out_path, html);
            } 
            // Otherwise copy directly (e.g., CSS, images)
            else {
                fs::create_directories(out_path.parent_path());
                fs::copy_file(entry.path(), out_path, fs::copy_options::overwrite_existing);
            }
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: slategen <src> <out> [template] [style]\n";
        return 1;
    }

    fs::path src = argv[1];
    fs::path dst = argv[2];
    std::string tmpl;
    std::string style_path = (argc > 4) ? argv[4] : "style.css";

    // Load template file if provided, else use default HTML structure
    if (argc > 3) tmpl = read_file(argv[3]);
    else tmpl = R"(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>{{title}}</title>
<link rel="stylesheet" href="{{style}}">
</head>
<body>
<main>
{{content}}
</main>
</body>
</html>)";

    // Start processing the site
    process_directory(src, dst, tmpl, style_path);

    std::cout << "Site generated successfully.\n";
    return 0;
}

