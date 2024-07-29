"""Rules for extracting a VST SDK tracked with git LFS"""

def _vst3_sdk_archive(repository_ctx):
    repository_ctx.extract(repository_ctx.attr.src, stripPrefix = repository_ctx.attr.src.name.split(".zip")[0])
    repository_ctx.execute(["chmod", "777", "./**"])
    repository_ctx.file("BUILD.bazel", repository_ctx.read(repository_ctx.attr.build_file))

vst3_archive = repository_rule(
    attrs = {
        "src": attr.label(mandatory = True, allow_single_file = True),
        "build_file": attr.label(mandatory = True, allow_single_file = True),
    },
    implementation = _vst3_sdk_archive,
)
