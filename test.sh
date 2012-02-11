for(( i = 0; i < 1000; i++ ))
do
	./release/client -i 192.168.1.72 -l 1000 -t 1000 -p 81001 &
done
