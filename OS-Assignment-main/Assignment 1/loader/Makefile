#Create lib_simpleloader.so from loader.c
all:
	gcc -shared -m32 -o lib_simpleloader.so -fPIC loader.c
	@mkdir -p ../bin
	@mv lib_simpleloader.so ../bin/lib_simpleloader.so

#Provide the command for cleanup
