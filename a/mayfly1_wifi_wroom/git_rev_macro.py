import subprocess

git_rev = str(subprocess.check_output(["git", "rev-parse","--abbrev-ref", "HEAD"]).strip())
git_usr = str(subprocess.check_output(["git", "config","user.name"]).strip())

print('-DPIO_SRC_REV={'+(''.join('0x%02x,' % ord(c) for c in git_rev) ) +'0x0}')
print('-DPIO_SRC_USR={'+(''.join('0x%02x,' % ord(c) for c in git_usr) ) +'0x0}')