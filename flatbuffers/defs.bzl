def _flatc_toolchain_impl(ctx):
    return [platform_common.ToolchainInfo(
        flatc_binary = ctx.executable.binary,
    )]

flatc_toolchain = rule(
    implementation = _flatc_toolchain_impl,
    doc = "Flatc linux toolchain",
    attrs = {
        "binary": attr.label(
            doc = "Flatc binary label",
            cfg = "exec",
            executable = True,
        ),
    },
)

def _rust_flatbuffer_impl(ctx):
    flatc_toolchain = ctx.toolchains["//flatbuffers:flatc_toolchain_type"]

    args = ctx.actions.args()

    output_file_name = "{}_generated.rs".format(ctx.file.schema.basename.split(".")[0])
    output_file = ctx.actions.declare_file(output_file_name)

    args.add(flatc_toolchain.flatc_binary)
    args.add(ctx.file.schema)
    args.add(output_file.dirname)

    ctx.actions.run(
        inputs = [flatc_toolchain.flatc_binary, ctx.file.schema],
        outputs = [output_file],
        executable = ctx.executable._executable,
        arguments = [args],
    )

    return DefaultInfo(
        files = depset([output_file]),
    )

rust_flatbuffer_gen = rule(
    implementation = _rust_flatbuffer_impl,
    doc = "Runs flatc to generate rust module",
    attrs = {
        "schema": attr.label(
            doc = "Flatbuffer schema",
            allow_single_file = [".fbs"],
        ),
        "_executable": attr.label(
            doc = "Flatc generation script label",
            cfg = "exec",
            executable = True,
            default = Label("//flatbuffers:run_flatc"),
        ),
    },
    toolchains = [
        "//flatbuffers:flatc_toolchain_type",
    ],
)
