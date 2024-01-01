#/bin/bash
for i in `find src/ -type f -not \( -path "src/.inc/*" -o -path "src/waveshare/*" \)`; do grep TODO $i && echo $i; done
