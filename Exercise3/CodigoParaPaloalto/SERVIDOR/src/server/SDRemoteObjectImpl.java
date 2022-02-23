/**
* RemoteMethodsImpl.java
* 
* Alberto Miguel Diez - amigud00[at]estudiantes.unileon.es
* 
* Code based on the implementation provided by:
* All rights reserved (C) 2014-2021 by José María Foces Morán and José María
* Foces Vivancos from textbook "Conceptual Computer Networks"
*
* This class offers two example public methods to be exported. Objects of this
* class become RMI remote objects since this class implements a java.rmi.Remote
* subinterface (SDRemoteObject is the subinterface)
*
*****************************************************************************
*/
package dsrmipract;

import java.rmi.*;
import java.rmi.server.UnicastRemoteObject;

public class SDRemoteObjectImpl extends UnicastRemoteObject
implements SDRemoteObject {

  private long counter = 0;

  public SDRemoteObjectImpl(int port) throws RemoteException {
    super(port);
  }

  /*
  * This method increases the counter by 1
  */
  public long increment() throws RemoteException {

    counter++;

    printThread();

    return counter;

  }

  /*
  * Returns the factorial of x
  */
  public long decrement() throws RemoteException {

    counter--;

    printThread();

    return counter;

  }
  
  private void printThread() {
    System.out.println("Thread name:  " + Thread.currentThread().getName());
    System.out.flush();
  }
}
