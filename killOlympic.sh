kill -9 $(ps aux | grep olympic | grep -v "grep" | cut -d " " -f10)
