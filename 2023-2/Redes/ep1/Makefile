all : broker

broker : broker.c queue.c utils.c state_machine.c
	@gcc -o broker broker.c queue.c utils.c state_machine.c

docker:
	docker run -d --hostname rmq --name rabbit-server -p 8080:15672 -p 5672:5672 rabbitmq:3-management

clean:
	rm broker
