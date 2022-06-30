#%%
import json
import os

in_json = open(os.environ["GITHUB_WORKSPACE"] + "/arduino_lint.json")
arduino_lint_results = json.load(in_json)

out_md = open(os.environ["GITHUB_WORKSPACE"] + "/arduino_lint.md", "w+")

out_md.write("## Results of testing examples\n\n")
out_md.write(" | Path | Project Type | Result | Problems | \n")
out_md.write(" | --- | --- | --- | --- | \n")
for project in arduino_lint_results["projects"]:
    fail_list = "<ul>"
    for failed_rule in project["rules"]:
        fail_list += "<li> **{}** - {}</li>".format(
            failed_rule["level"], failed_rule["message"]
        )
    fail_list += "</ul>"
    out_md.write(
        " | {} | {} | {} | {} | \n".format(
            project["path"]
            .replace(os.environ.get("GITHUB_WORKSPACE"), "")
            .replace("examples\\", ""),
            project["projectType"],
            ":white_check_mark:" if project["summary"]["pass"] else ":x:",
            fail_list,
        )
    )

#%%
