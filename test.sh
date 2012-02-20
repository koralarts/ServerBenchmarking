for(( i = 0; i < 1000; i++ ))
do
	./release/client -i 127.0.0.1 -l 1000 -t 1 &
done
