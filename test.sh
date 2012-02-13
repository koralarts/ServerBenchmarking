for(( i = 0; i < 1000; i++ ))
do
	./release/client -i 192.168.0.3 -l 1000 -t 1000 &
done
