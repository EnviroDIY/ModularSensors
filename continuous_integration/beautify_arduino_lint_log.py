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
        if failed_rule["result"] != "pass":
            fail_list += "<li>{}{}<ul>".format(
                failed_rule["ID"],
                " - {}".format(failed_rule["brief"])
                if failed_rule["brief"] != ""
                else "",
            )
            fail_list += "<li> **{}**{}</li>".format(
                failed_rule["level"],
                " - {}".format(failed_rule["message"].replace("\n", "<br/><br/>"))
                if failed_rule["message"] != ""
                else "",
            )
            fail_list += "</ul></li>"
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
