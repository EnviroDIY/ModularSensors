import subprocess

git_rev = str(subprocess.check_output(["git", "rev-parse","--abbrev-ref", "HEAD"]).strip())

print('-DPIO_SRC_REV={'+(''.join('0x%02x,' % ord(c) for c in git_rev) ) +'0x0}')