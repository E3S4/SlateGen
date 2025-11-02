#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>
#include <regex>

namespace fs = std::filesystem;

// Simple Markdown → HTML converter (basic headers, bold, italics, lists)
std::string markdownToHTML(const std::string& markdown) {
    std::string html = markdown;

    // headers
    html = std::regex_replace(html, std::regex(R"(^# (.*))", std::regex::multiline), "<h1>$1</h1>");
    html = std::regex_replace(html, std::regex(R"(^## (.*))", std::regex::multiline), "<h2>$1</h2>");
    html = std::regex_replace(html, std::regex(R"(^### (.*))", std::regex::multiline), "<h3>$1</h3>");

    // bold + italic
    html = std::regex_replace(html, std::regex(R"(\*\*(.*?)\*\*)"), "<strong>$1</strong>");
    html = std::regex_replace(html, std::regex(R"(\*(.*?)\*)"), "<em>$1</em>");

    // unordered lists
    html = std::regex_replace(html, std::regex(R"(^- (.*))", std::regex::multiline), "<li>$1</li>");
    html = std::regex_replace(html, std::regex(R"((<li>.*</li>)+)"), "<ul>$&</ul>");

    // paragraphs
    html = std::regex_replace(html, std::regex(R"((^|\n)([^<\n][^\n]*)(\n|$))"), "$1<p>$2</p>$3");

    return html;
}

// Read file into string
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("Failed to open file: " + path);
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// Write string to file
void writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) throw std::runtime_error("Failed to write file: " + path);
    file << content;
}

// Inject Markdown HTML into template
void convertMarkdownToHtml(const std::string& inputFile, const std::string& outputFile) {
    std::string markdown = readFile(inputFile);
    std::string content = markdownToHTML(markdown);
    std::string templateHtml = readFile("template.html");

    // title = filename without extension
    std::string title = fs::path(inputFile).stem().string();
    templateHtml = std::regex_replace(templateHtml, std::regex(R"(\{\{title\}\})"), title);
    templateHtml = std::regex_replace(templateHtml, std::regex(R"(\{\{content\}\})"), content);

    writeFile(outputFile, templateHtml);
    std::cout << "Converted: " << inputFile << " -> " << outputFile << std::endl;
}

// Help message
void printHelp() {
    std::cout << "SlateGen - Minimal Static Site Generator\n";
    std::cout << "Usage:\n";
    std::cout << "  slategen <input> <output>\n";
    std::cout << "    <input>  : Markdown file or directory\n";
    std::cout << "    <output> : Output file or directory\n\n";
    std::cout << "Examples:\n";
    std::cout << "  slategen src/ output/            Convert all .md files in src/ to HTML\n";
    std::cout << "  slategen about.md about.html     Convert a single file\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help   Show this help message\n";
}

int main(int argc, char* argv[]) {
    try {
        if (argc < 2 || std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help") {
            printHelp();
            return 0;
        }

        fs::path inputPath = argv[1];
        fs::path outputPath = argv[2];

        if (fs::is_directory(inputPath)) {
            fs::create_directories(outputPath);
            for (const auto& entry : fs::recursive_directory_iterator(inputPath)) {
                if (entry.path().extension() == ".md") {
                    fs::path relative = entry.path().filename();
                    fs::path outFile = outputPath / (relative.stem().string() + ".html");
                    convertMarkdownToHtml(entry.path().string(), outFile.string());
                }
            }
        } else if (fs::is_regular_file(inputPath)) {
            convertMarkdownToHtml(inputPath.string(), outputPath.string());
        } else {
            std::cerr << "Invalid input path: " << inputPath << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

