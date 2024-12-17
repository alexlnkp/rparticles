#define NOB_IMPLEMENTATION
#include "nob.h"

#define STR_OR_DEFAULT(str, def) ((str) ? (str) : (def))

#define DEFAULT_CC "gcc"

int main(int argc, char* argv[]) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    Nob_Cmd cmd = {0};

    nob_mkdir_if_not_exists("out");

    char* CC = getenv("CC");
    nob_cmd_append(&cmd, STR_OR_DEFAULT(CC, DEFAULT_CC));
    nob_cmd_append(&cmd, "-Wall", "-Wextra");
    nob_cmd_append(&cmd, "-std=c99");
    nob_cmd_append(&cmd, "-o", "out/1");
    nob_cmd_append(&cmd, "examples/1.c");
    nob_cmd_append(&cmd, "-Iinclude");
    nob_cmd_append(&cmd, "-lraylib");

    return nob_cmd_run_sync(cmd);
}
