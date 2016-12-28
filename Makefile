JAVACLASSPATH := ${CURDIR}/realHTMLconnector.jar:/srv/apache-tomcat-8.5.9/lib/servlet-api.jar

INCLUDES = -I/usr/java/jdk1.8.0_111/include/ \
		   -I/usr/java/jdk1.8.0_111/include/linux/ \
		   -I./jniLibrary/

CC = gcc

all:
	make jni_connector
	make tomcat_servlet
	make jni_so

jni_connector:
	javac realHTML/tomcat/connector/JNINatural.java
	javac -cp ./ realHTML/tomcat/connector/JNILoader.java
	javac -cp ./ realHTML/tomcat/connector/Router.java
	javac -cp ./ realHTML/tomcat/connector/ConfigurationLoader.java
	jar cf realHTMLconnector.jar realHTML

tomcat_servlet:
	javac -cp ${JAVACLASSPATH} servlet/realHTMLServlet.java

deploy_servlet:
	make tomcat_servlet
	cp ./servlet/realHTMLServlet.class /srv/apache-tomcat-8.5.9/webapps/ROOT/WEB-INF/classes/

jni_so:
	javah -jni -o jniLibrary/realHTML_tomcat_connector_JNINatural.h \
			-cp ./ realHTML.tomcat.connector.JNINatural
	${CC} -c -fpic $(INCLUDES) -o ./jniLibrary/callNatural.o ./jniLibrary/callNatural.c
	${CC} -shared -o librealHTMLconnector.so ./jniLibrary/callNatural.o

deploy_all:
	make all
	cp ./realHTMLconnector.jar /srv/apache-tomcat-8.5.9/lib/
	cp ./servlet/realHTMLServlet.class /srv/apache-tomcat-8.5.9/webapps/ROOT/WEB-INF/classes/
	/srv/apache-tomcat-8.5.9/bin/catalina.sh stop
	export realHTMLconfiguration=/srv/tomcat_connector/config_files/config.xml \
		&& /srv/apache-tomcat-8.5.9/bin/catalina.sh start

test:
	cd tests && \
		javac -cp ${JAVACLASSPATH} ConfigTest.java  && \
		java -cp ${JAVACLASSPATH}:./ ConfigTest

