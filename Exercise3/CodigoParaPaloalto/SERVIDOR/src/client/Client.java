/**
* ***************************************************************************
* Client.java
* 
* Alberto Miguel Diez - amigud00[at]estudiantes.unileon.es
* 
* Code based on the implementation provided by:
* All rights reserved (C) 2014-2021 by José María Foces Morán and José María
* Foces Vivancos from textbook "Conceptual Computer Networks"
*
* This class is the client that will invoke the example remote methods.
*
* PRECONDITIONS FOR EXECUTION OF THIS PROGRAM
* -------------------------------------------
*
* This program needs that a security policy file be specified in the java command
* line, that file should contain a java security policy file that allows the
* run time to dynamically load java code from anywhere, specifically, it needs
* to download and execute the proxy classes from the remote server by way of
* the remote registry. In order for the user to express this permission, a
* command line like the following one would be fine:
*
* $ java -Djava.security.policy="all.policy"\
    sdrmiexample.Client\
    192.168.2.33\
    <Remote object registry name>
*
* The policy file (all.policy) contents required in this case would be:
*
* grant{
*       permission java.security.AllPermission;
* };
******************************************************************************/

package dsrmipract;

import java.rmi.*;
import java.rmi.registry.*;
import java.util.logging.Level;
import java.util.logging.Logger;

public class Client {

  public static void main(String args[]) {

    if(args.length != 3){
      System.err.println("dsrmipract.Client <ip address of rmi server's registry> <port of rmiregistry> <rmi name>");
      System.exit(-1);
    }

    /*
    * The security manager will use the policy file mentioned above
    */
    if (System.getSecurityManager() == null) {
      System.setSecurityManager(new SecurityManager());
    }


    try {
      /* Locate an rmi registry at the IP address specified in the command
      * line
      */
      Registry r = LocateRegistry.getRegistry(args[0], Integer.parseInt(args[1]));
      System.out.println("registry found");
      System.out.flush();

      SDRemoteObject cc = (SDRemoteObject) r.lookup(args[2]);

      System.out.println("stub lookup done");
      System.out.flush();

      // Invocation of remote methods
      
      System.out.println("Incrementing the counter: "+ cc.increment());
      System.out.println("Incrementing the counter: " + cc.increment());
      System.out.println("Decrementing the counter: "+ cc.decrement());
      System.out.flush();

    } catch (RemoteException ex) {
      Logger.getLogger(Client.class.getName()).log(Level.SEVERE, null, ex);
    } catch (NotBoundException ex) {
      Logger.getLogger(Client.class.getName()).log(Level.SEVERE, null, ex);
    }

  }
}
