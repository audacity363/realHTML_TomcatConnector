#JAVACLASSPATH = ${CURDIR}/servlet/web/WEB-INF/lib/realHTMLconnector.jar:/srv/apache-tomcat-8.5.9/lib/servlet-api.jar
JAVACLASSPATH = /u/it/a140734/rh4n/realHTML_TomcatConnector/servlet/web/WEB-INF/lib/realHTMLconnector.jar:/opt/tomcat/apache-tomcat-8.0.41/lib/servlet-api.jar


#INCLUDES = -I/usr/java/jdk1.8.0_111/include/ \
#		   -I/usr/java/jdk1.8.0_111/include/linux/ \
#		   -I./jniLibrary/

INCLUDES = -I/SAG/cjp/v16/include/ \
		   -I./jniLibrary/include/


#CC = gcc
CC = xlc

all:
	@printf "targets:\n"
	@printf "\tcomplete      : Build the complete connector\n"
	@printf "\twarfile       : Pack the .war file for deployment\n"
	@printf "\tjni_connector : Compile the utility library\n"
	@printf "\ttomcat_servlet: Compile the tomcat servlet\n"
	@printf "\tjni_so        : Compile the JNI so object\n"

jni_connector:
	mkdir -p ./servlet/web/WEB-INF/lib
	javac -classpath "./servlet/src/" ./servlet/src/realHTML/tomcat/connector/RH4NReturn.java
	javac -classpath "./servlet/src/" ./servlet/src/realHTML/tomcat/connector/RH4NParams.java
	javac -classpath "./servlet/src/" ./servlet/src/realHTML/tomcat/connector/JNINatural.java
	javac -classpath "./servlet/src/" ./servlet/src/realHTML/tomcat/connector/JNILoader.java
	javac -classpath "./servlet/src/" ./servlet/src/realHTML/tomcat/connector/Router.java
	javac -classpath "./servlet/src/" ./servlet/src/realHTML/tomcat/connector/ConfigurationLoader.java
	javac -classpath "./servlet/src/" ./servlet/src/realHTML/tomcat/connector/RH4NCallParms.java
	cd ./servlet/src/ && jar cf ../../servlet/web/WEB-INF/lib/realHTMLconnector.jar ./realHTML

tomcat_servlet:
	mkdir -p ./servlet/web/WEB-INF/classes
	javac -cp $(JAVACLASSPATH) servlet/src/realHTMLServlet.java
	cp ./servlet/src/realHTMLServlet.class ./servlet/web/WEB-INF/classes/

warfile:
	cd ./servlet/web/ && jar cvf ../realHTML4Natural.war .
	@printf "Created the realHTML4Natural.war file in ./servlet\n"

complete:
	make jni_connector
	make tomcat_servlet
	make warfile

deploy_servlet:
	make tomcat_servlet
	cp ./servlet/realHTMLServlet.class /srv/apache-tomcat-8.5.9/webapps/ROOT/WEB-INF/classes/

jni_so:
	rm -f librealHTMLconnector.so
	cd ./servlet/src && javah -jni -o ../../jniLibrary/include/realHTML_tomcat_connector_JNINatural.h \
			realHTML.tomcat.connector.JNINatural
	${CC} -c -g -fpic $(INCLUDES) -o ./jniLibrary/bin/main.o ./jniLibrary/src/main.c
	${CC} -c -g -fpic $(INCLUDES) -o ./jniLibrary/bin/natural.o ./jniLibrary/src/natural.c
	#${CC} -shared -o librealHTMLconnector.so ./jniLibrary/callNatural.o
	${CC} -G -o librealHTMLconnector.so ./jniLibrary/bin/*.o

test:
	javac -cp ./servlet/src/ ./tests/CallNatural.java 
	java -cp "./servlet/src/:./tests/" -Djava.library.path="/u/it/a140734/rh4n/realHTML_TomcatConnector/" CallNatural

clean:
	rm -f ./servlet/realHTMLServlet.class
	rm -f ./servlet/realHTML/tomcat/connector/*.class
	rm -f ./servlet/realHTML4Natural.war
	rm -f ./servlet/web/WEBINF/classes/realHTMLServlet.class
	rm -f ./servlet/web/WEB-INF/lib/realHTMLconnector.jar
	rm -f ./librealHTMLconnector.so
	rm -f ./tests/*.class
	rm -f ./jniLibrary/*.o
