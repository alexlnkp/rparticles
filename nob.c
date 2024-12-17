#define NOB_IMPLEMENTATION
#include "nob.h"

#define STR_OR_DEFAULT(str, def) ((str) ? (str) : (def))

#define NUM_EXAMPLES 2

#define DEFAULT_CC "gcc"

void build_examples(Nob_Cmd* cmd) {
    char* CC = getenv("CC");

    char *out_name = alloca(256 * sizeof(char));
    char *source_path = alloca(256 * sizeof(char));

    for (int i = 1; i <= NUM_EXAMPLES; ++i) {
        nob_cmd_append(cmd, STR_OR_DEFAULT(CC, DEFAULT_CC));
        nob_cmd_append(cmd, "-Wall", "-Wextra");
        nob_cmd_append(cmd, "-std=c99");

        sprintf(out_name, "out/%d", i);
        nob_cmd_append(cmd, "-o", out_name);

        sprintf(source_path, "examples/%d.c", i);
        nob_cmd_append(cmd, source_path);

        nob_cmd_append(cmd, "-Iinclude", "-I.");
        nob_cmd_append(cmd, "-lraylib");
        nob_cmd_run_sync_and_reset(cmd);
    }
}

int main(int argc, char* argv[]) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    Nob_Cmd cmd = {0};

    nob_mkdir_if_not_exists("out");
    build_examples(&cmd);

    return 0;
}
