for(( i = 0; i < $1; i++ ))
do
	./release/client -i 192.168.0.3 -l 1000 -t 10 &
done
