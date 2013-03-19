#include <string>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

#ifdef WIN32
# define mkdir(path, mode) _mkdir(path)
#else
# include <sys/stat.h>
#endif

int traverse(const char *src, const char *dst, const char *but_not)
{
\tDIR *d = opendir(src);
\tif (!d) {
\t\tfprintf(stderr, "Unable to open source directory `%s'.\n", src);
\t\treturn 1;
\t}

\tint ret = 0;
\tstruct dirent *ent;
\tfor (ent = readdir(d); ent; ent = readdir(d)) {
\t\tif (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..") || !strcmp(ent->d_name, but_not)) {
\t\t\tcontinue;
\t\t}

\t\tstd::string path(src);
\t\tpath.push_back('/');
\t\tpath.append(ent->d_name);

\t\tstruct stat st;
\t\tif (stat(path.c_str(), &st)) {
\t\t\tfprintf(stderr, "Unable to stat `%s`.\n", path.c_str());
\t\t\tret = 1;
\t\t\tcontinue;
\t\t}

\t\tstd::string dstpath(dst);
\t\tdstpath.push_back('/');
\t\tdstpath.append(ent->d_name);

\t\tif (S_ISDIR(st.st_mode)) {
\t\t\tmkdir(dstpath.c_str(), 0755);
\t\t\tret |= traverse(path.c_str(), dstpath.c_str(), but_not);
\t\t} else {
\t\t\tstd::ifstream inf(path.c_str(), std::ios_base::in|std::ios_base::binary);
\t\t\tstd::ofstream outf(dstpath.c_str(), std::ios_base::out|std::ios_base::binary);
\t\t\twhile (inf.good()) {
\t\t\t\tchar buf[8192];
\t\t\t\tsize_t n = inf.readsome(buf, 8192);
\t\t\t\tif (n > 0) outf.write(buf, n);
\t\t\t\tif (n == 0) break;
\t\t\t}
\t\t}
\t}

\treturn ret;
}

int main(int argc, char **argv)
{
\tif (argc != 4) {
\t\tfprintf(stderr, "Usage: copy_but <source> <destination> <but>\n");
\t\treturn 1;
\t}

\tchar *src = argv[1];
\tchar *dst = argv[2];
\tchar *but_not = argv[3];

\tmkdir(dst, 0755);

\treturn traverse(src, dst, but_not);
}

