#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
\tchar *input, *name, *output;
\tFILE *inf, *outf;
\tsize_t total;

\tif (argc != 4) {
\t\tfprintf(stderr, "Usage: embed <input> <name> <output>\n");
\t\treturn 1;
\t}

\tinput = argv[1];
\tname = argv[2];
\toutput = argv[3];

\tinf = fopen(input, "rb");
\tif (!inf) {
\t\tfprintf(stderr, "Unable to open `%s' for input.\n", input);
\t\treturn 1;
\t}

\toutf = fopen(output, "wb");
\tif (!outf) {
\t\tfprintf(stderr, "Unable to open `%s' for output.\n", output);
\t\treturn 1;
\t}

\tfprintf(outf, "#pragma once\n");
\tfprintf(outf, "const unsigned char %s_data[] = {\n", name);

\ttotal = 0;
\twhile (!feof(inf)) {
\t\tunsigned char buf[16];
\t\tsize_t n = fread(buf, 1, 16, inf);
\t\tif (n > 0) {
\t\t\tsize_t i;
\t\t\tfor (i = 0; i < n; i++) {
\t\t\t\tfprintf(outf, "0x%02x, ", buf[i]);
\t\t\t}
\t\t\ttotal += n;
\t\t\tfprintf(outf, "\n");
\t\t}
\t}

\tfprintf(outf, "};\n");
\tfprintf(outf, "const int %s_size = %d;\n", name, (int)total);

\tfclose(inf);
\tfclose(outf);

\treturn 0;
}

