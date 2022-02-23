/** 
* SDRemoteObject.java
* 
* Alberto Miguel Diez - amigud00[at]estudiantes.unileon.es
* 
* Code based on the implementation provided by:
* All rights reserved (C) 2014-2021 by José María Foces Morán and José María
* Foces Vivancos from textbook "Conceptual Computer Networks"
*
* This is the java interface file corresponding to the remote interface of
* RemoteMethodsImpl objects, the compiled form of this file (.class) is
* the only local file needed by the client plus the client class itself.
*
* java.rmi.Remote is a marker interface. Every method declared in this interface
* file will become a remote method for objects that implement it.
*
******************************************************************************/
package dsrmipract;

import java.rmi.*;

public interface SDRemoteObject extends Remote{

  public long increment() throws RemoteException;

  public long decrement() throws RemoteException;

}
