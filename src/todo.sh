#/bin/bash
for i in `find . -type f -not \( -path "./.inc/*" -o -path "./waveshare/*" \)`; do grep TODO $i && echo $i; done
