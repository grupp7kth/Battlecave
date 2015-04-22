import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.util.*;
import java.net.*;
import java.io.*;

public class DogfightServer {
	ArrayList<Dogfighter> clients;
	ArrayList<RegisteredUser> reglist;
	DatagramPacket sandPaket;
	byte[] bytebuf;
	DogfightDataPacket speldata;
	long lastNap = System.currentTimeMillis();
	int CPS;
	static Object lock = new Object();
	int tcpPort, udpOut,udpIn;
	final int SPAMS_PER_SECOND = 100, VIRTUAL_SCREEN_RESOLUTION = 100000;
	final String FILENAME = "regusers.qbt";
	
	public DogfightServer(int tcpPort,int c) {
		// inPort = UDP-porten som servern tar emot paket till. Denna automatgenereras i InstreamReader.
		// outPort = UDP-porten som servern skickar paket från. Denna automatgenereras i DatapacketSpammer.
		// tcpPort = Local TCP port för TCP-kommunikation. Denna bestäms i förväg.
		CPS = c;
		this.tcpPort = tcpPort;
		initiateFields();
		reglist = readUserFile();
		new DatapacketSpammer(SPAMS_PER_SECOND).start();
		new InstreamReader().start();
		new ClientListener(tcpPort).start();
		try {
			System.out.println("Dogfighter Server at IP "+InetAddress.getLocalHost().getHostAddress()+", TCP port "+tcpPort);
		}
		catch (Exception e) {
			System.out.println("Absolute and total failure: "+e);
			System.exit(1);
		}

		while (true) {
			scanClientArray();
			updatePositions();
			sleep();
		}
	}

	private class DatapacketSpammer extends Thread {
		int spamsPerSecond;
		DatagramSocket outStream;
		public DatapacketSpammer(int sps) {
			spamsPerSecond = sps;
			try {
				outStream = new DatagramSocket();
			}
			catch (Exception e) {
				System.out.println("MainThread failed opening outstreamport: "+e);
			}
			finally {
				udpOut = outStream.getLocalPort();
				System.out.println("UDP outport: "+udpOut);
			}
		}
		public void run() {
			while (true) {
				bytebuf = speldata.getBytes();
				sandPaket.setData(bytebuf);
				if ((bytebuf.length) != 904) {
					System.out.println("Server mainThread: Error in data length.");
					System.exit(1);
				}
				synchronized(lock) {
					for (Dogfighter user : clients) {
						sandPaket.setAddress(user.adress);
						sandPaket.setPort(user.inPort);
						try {
							outStream.send(sandPaket);
						}
						catch (IOException e) {
							System.out.println("Server / mainthread: Error sending package to user "+user);
						}
					}
				}
				try {
					Thread.sleep(1000/spamsPerSecond);
				}
				catch (Exception e) {}
			}
		}
	}
	
	public void scanClientArray() {
		for (Dogfighter dog : clients) {
			if (dog.hoger)
				dog.skepp.rotate(-1.0/CPS);
			if (dog.vanster)
				dog.skepp.rotate(1.0/CPS);
			if (dog.gas) {
				dog.skepp.accelerate(CPS);
			}
			if (dog.skjut && dog.skepp.canShoot()) {
				System.out.println("Server / mainthread: "+dog+" skjuter!");
				Bullet b = speldata.findOffBullet();
				b.turnOn(dog.skepp.shoot(),true, dog);
//				System.out.println(b);
			}
			if (dog.getName()!="noName" && !dog.skepp.isOn()) {
				dog.millisSinceKilled+=1000/CPS;
				if (dog.millisSinceKilled > Dogfighter.respawntime) {
					System.out.println(dog+" respawns.");
					dog.skepp.turnOn(VIRTUAL_SCREEN_RESOLUTION/2,VIRTUAL_SCREEN_RESOLUTION/2,dog.getName());
					dog.millisSinceKilled =0;
					
				}
			}
		}
	}
	
	public void updatePositions() {
		for (Ship s : speldata.skepp ) {
			if (s != null) {
				s.move(CPS);
			}
		}
		for (Bullet b : speldata.skott ) {
			if (b != null) {
				b.move(CPS);
				if (new Point(b.getxPos(),b.getyPos()).distance(VIRTUAL_SCREEN_RESOLUTION/2,VIRTUAL_SCREEN_RESOLUTION/2) < VIRTUAL_SCREEN_RESOLUTION*0.05) {
					b.turnOff();
				}
			}
		}
		for (Ship s : speldata.skepp) {
			// COLLISIONS SHIP/BULLET
			for (Bullet b : speldata.skott) {
				if (s != null && b!= null) {
					if (b.on && s.on) {
						if (s.revalidateBody(VIRTUAL_SCREEN_RESOLUTION).contains(b.getxPos(),b.getyPos())) {
							System.out.println(s.name+" was hit by "+b.owner.getName()+"!");
							calculateAndSendScore(s,b);
							s.turnOff();
						}
					}
				}
			}
			// COLLISIONS SHIP/SHIP
			for (Ship s2 : speldata.skepp) {
				if (s !=null && s2 != null) {
					if (s.on && s2.on && s!= s2) {
						Polygon subject = s.revalidateBody(VIRTUAL_SCREEN_RESOLUTION);
						Polygon object = s2.revalidateBody(VIRTUAL_SCREEN_RESOLUTION);
						for (int n = 0; n<object.npoints; n++) {
							if (subject.contains(object.xpoints[n],object.ypoints[n])) {
								int deltaX = s.getxPos()-s2.getxPos();
								int deltaY = s.getyPos()-s2.getyPos();
								int totVelX = s.getxVel()-s2.getxVel();
								int totVelY = s.getyVel()-s2.getyVel();
								int deltaR =1;
								if (deltaX!=0) deltaR = deltaY/deltaX;
//								int subjectWeight = Math.pow(subject.storlek,3);
								int subjectWeight = 1;
//								int objectWeight = Math.pow(object.storlek,3);
								int objectWeight = 1;
								int weightConstant = 2/(subjectWeight+objectWeight);
								int bX = (int)((totVelX+totVelY*deltaR)/(1+Math.pow(deltaR,2)));
								int bY = bX*deltaR;
								
								s.setxVel(totVelX-weightConstant*objectWeight*bX + s2.getxVel());
								s.setyVel(totVelY-weightConstant*objectWeight*bY + s2.getyVel());
								s2.setxVel(weightConstant*subjectWeight*bX + s2.getxVel());
								s2.setyVel(weightConstant*subjectWeight*bY + s2.getyVel());
								s.move(CPS);
								s2.move(CPS);
							}
						}
					}
				}
			}
		}
	}
	private void calculateAndSendScore(Ship skepp, Bullet skott) {
		if (skepp.owner == skott.owner) return;
		int delta = skepp.owner.poang-skott.owner.poang+1000;
		int points = delta/100;
		skepp.owner.poang -=points;
		skott.owner.poang +=points;
		for (Dogfighter d : clients) {
			d.getOutputStream().println(skepp.name+";"+skepp.owner.poang);
			d.getOutputStream().println(skott.owner.getName()+";"+skott.owner.poang);
		}
		saveUsers(reglist);
	}

	public void sleep () {
		int napTime = (int)(1000/CPS+lastNap-System.currentTimeMillis());
		if (napTime <1) {
			System.out.println("Server / mainThread warning: "+napTime+" millis sleeptime!");
		}
		else {
			try{
				Thread.sleep(napTime);
			}
			catch (InterruptedException e) {
			}
		}
		lastNap = System.currentTimeMillis();
	}
				
	public void initiateFields() {
		clients = new ArrayList<Dogfighter>();
		speldata = new DogfightDataPacket();
		sandPaket = new DatagramPacket(speldata.data,speldata.data.length);
//		speldata.skepp[0].turnOn((int)(VIRTUAL_SCREEN_RESOLUTION*0.42333),(int)(VIRTUAL_SCREEN_RESOLUTION*0.32111),"Dummyship");
//		speldata.skepp[0].setFacingInDegrees(95);
//		speldata.skepp[0].setxVel(VIRTUAL_SCREEN_RESOLUTION/10);
//		speldata.skepp[0].setyVel(VIRTUAL_SCREEN_RESOLUTION/10);
	}
	
	public ArrayList<RegisteredUser> readUserFile() {
		File f = new File(FILENAME);
		Scanner sc = null;
		try {
			sc = new Scanner(f);
		}
		catch (FileNotFoundException e) {}
		ArrayList<RegisteredUser> temp = new ArrayList<RegisteredUser>();
		if (sc != null) {
			while (sc.hasNext()) {
			temp.add(new RegisteredUser(sc.nextLine(),sc.nextLine(),Integer.parseInt(sc.nextLine())));
			}
			sc.close();
		}
		return temp;
	}
	
	public void saveUsers(ArrayList<RegisteredUser> list) {
		File f = new File(FILENAME);
		PrintWriter pw = null;
		try {
			pw = new PrintWriter(f);
		}
		catch (FileNotFoundException e) {}
		for (RegisteredUser r : list) {
			pw.println(r);
		}
		pw.close();
	}

	public static void main(String[] args) {
		new DogfightServer(4444,200);
		// TCP port, Calculations per second.
	}
	
	protected class ClientListener extends Thread {
		ServerSocket ear;

		public ClientListener(int port) {
			try {
				ear = new ServerSocket(port);
			}
			catch (Exception e) {
				System.out.println("Server / ClientListener error: couldn't open port: "+e);
			}
		}

		public void run() {
			while (true) {
				Socket socka = null;
				try {
					socka = ear.accept();
				}
				catch (IOException e) {
					System.out.println("Server / ClientListener error: Fel i klientkontakten: "+e);
				}
				new ClientThread(socka).start();
			}
		}
		protected class ClientThread extends Thread {
			Socket cs;
			BufferedReader in = null;
			PrintWriter out = null;
			Dogfighter spectator;

			public ClientThread(Socket s) {
				cs = s;
			}

			public void run() {
				handshakeClient();
				System.out.println("Server / ClientListener / ClientThread: Handshake finished. Now waiting participate signal.");
				while (true) {
					String instring = readAndCheckDisconnect();
					System.out.println("Server / ClientListener / ClientThread: Received: "+instring+" from "+spectator.getName());
					// Det jag får emot här är anropet om spektatorn loggar in. Innehåller Namn;losen;red;green;blue;UDPport_out
					String[] result = instring.split(";");
					
					// Kolla om det är en reggad användare och i så fall om lösenordet stämmer.
					
					boolean found = false, success = false;
					for (RegisteredUser ru : reglist) {
						if (result[0].equals(ru.namn)) {
							found = true;
							if (result[1].equals(ru.losen)) {
								spectator.poang = ru.poang;
								ru.owner = spectator;
								success = true;
							}
							else {
								System.out.println("Wrong password for user "+result[0]);
								spectator.getOutputStream().println("Error;password;incorrect");
							}
						}
					}
					if (found && !success) continue;
					if (!found) {
						reglist.add(new RegisteredUser(result[0],result[1],spectator));
						spectator.poang = 1000;
						System.out.println("Adding new user to list of registered users: "+result[0]);
						saveUsers(reglist);
					}
					int index =0;
					for (Ship s : speldata.skepp) {
						if (!s.on && s.owner == null) {
							spectator.index = index;
							spectator.setStartData(result[0],s);
							System.out.println("Ship "+index+" was free for use.");
							break;
						}
						index++;
					}
					int[] f = new int[3];
					for (int a = 0; a<3; a++) {
						f[a] = Integer.parseInt(result[a+2]);
					}
					spectator.skepp.turnOn((int)(VIRTUAL_SCREEN_RESOLUTION*0.5),(int)(VIRTUAL_SCREEN_RESOLUTION*0.5),spectator.getName());
					spectator.skepp.color = new Color(f[0],f[1],f[2]);
					// utPort är vilken port klienten skickar ifrån. Endast för igenkänningsanledningar.
					spectator.utPort = Integer.parseInt(result[5]);
					// Skicka ut vilken UDPport servern har för inkommande trafik, till användaren som just loggade in.
					spectator.getOutputStream().println(udpIn);
					// Skicka ut index, namn och skeppfärg till samtliga uppkopplade användare.
					for (Dogfighter d : clients) {
						sendShipInfo(spectator,d);
					}
				}
			}
			
			public void handshakeClient() {
				String instring = "";
				int inputport = 0;
				System.out.println("Server / ClientListener / ClientThread: Call heard. Trying to open streams to client.");
				try {
					in = new BufferedReader(new InputStreamReader(cs.getInputStream()));
					out = new PrintWriter(cs.getOutputStream(),true);
				}
				catch (Exception e) {
					System.out.println("Error opening streams: "+e);
					System.exit(0);
				}
				System.out.println("Server / ClientListener / ClientThread: Streams openened to "+cs+", trying to recieve UDP port from client.");
				try {
					instring = in.readLine();
				}
				catch (Exception e) {
					System.out.println("Error reading from client: "+e);
					System.exit(0);
				}

				System.out.println("Server / ClientListener / ClientThread: Have got the UDP port from client.");
				try {
					inputport = Integer.parseInt(instring);
				}
				catch (Exception e) {
					System.out.println("Error in message from client :"+e);
					System.exit(0);
				}
				// HERE, SOCKET IS SETUP AND UDP PORT MESSAGE IS RECEIVED CORRECTLY. CREATE DOGFIGHTER
				
				spectator = new Dogfighter(cs.getInetAddress(),inputport);
				System.out.println("New spectator: "+spectator+"; sending info on current stuff in game");
				spectator.setOutputStream(out);
				for (Ship s : speldata.skepp) {
					if (s.on) sendShipInfo(s.owner, spectator);
				}
				
				System.out.println("All info sent to "+spectator+". Now adding to spectatorlist.");
				synchronized(lock) {
					clients.add(spectator);
				}
			}
			
			public String readAndCheckDisconnect() {
				String inline = "";
				try{
					inline = in.readLine();
				}
				catch (Exception e) {
					System.out.println("Server / ClientListener / ClientThread: "+spectator+"disconnected? "+e);
					synchronized(lock) {
						clients.remove(spectator);
					}
					try {
						cs.close();
					}
					catch (Exception ee) {
						System.out.println("Couldn't even close the goddamn socket: "+ee);
					}
					if (spectator.index > -1) {
						speldata.skepp[spectator.index].logout();
						sendDisconnectAlertToAll(spectator.getName());
						spectator.disconnectShip();
					}
					stop();
				}
				if (inline == null) {
					System.out.println("Server / ClientListener / ClientThread: Got 'null'; client "+spectator+" disconnected?");
					synchronized(lock) {
						clients.remove(spectator);
					}
					try {
						cs.close();
					}
					catch (Exception ee) {
						System.out.println("Couldn't even close the goddamn socket: "+ee);
					}
					if (spectator.index > -1) {
						speldata.skepp[spectator.index].logout();
						sendDisconnectAlertToAll(spectator.getName());
						spectator.disconnectShip();
					}
					stop();
				}
				return inline;
			}
			
			public void sendShipInfo(Dogfighter subject, Dogfighter destination) {
				String sendstring = subject.index+";"+subject.getName()+";"+subject.poang+";"+subject.skepp.color.getRed()+";"+subject.skepp.color.getGreen()+";"+subject.skepp.color.getBlue();
				System.out.println("Sending "+sendstring+" to "+destination);
				destination.getOutputStream().println(sendstring);
			}
			public void sendDisconnectAlertToAll(String name) {
				System.out.println("Sending D/C alert about "+name);
				synchronized (lock) {
					for (Dogfighter d : clients) {
						d.getOutputStream().println(name+";off");
					}
				}
			}
		}
	}
				
	protected class InstreamReader extends Thread {
		byte[] b = {0};
		DatagramSocket inStream;
		DatagramPacket data = new DatagramPacket(b,1);
		
		public InstreamReader() {
			try {
				inStream = new DatagramSocket();
			}
			catch (Exception e) {
				System.out.println("Server/InstreamReader couldn't open port: "+e);
			}
			finally {
				udpIn = inStream.getLocalPort();
				System.out.println("UDP inport: "+udpIn);
			}
		}

		public void run() {
			while (true) {
				try {
					inStream.receive(data);
				}
				catch (IOException e) {
					System.out.println("Server / InstreamReader: Error receiving: "+e);
				}
//				System.out.println("Server: InstreamReader: Got something from someone!");
				Dogfighter caller = detectUser(data);
				if (caller != null) {
					if (caller.index > -1) {
//						System.out.println("Server / Instreamreader: Setting data: "+data.getData()[0]);
						caller.setData(data.getData()[0]);
					}
					else System.out.println("Server /InstreamReader: Non-joined client: "+caller+"; packet discarded.");
				}
				else System.out.println("Server / Instreamreader: Unknown user: "+data.getAddress()+";"+data.getPort()+"; packet discarded.");
			}
		}
		
		public Dogfighter detectUser(DatagramPacket p) {
			for (Dogfighter d : clients) {
//				System.out.println("Checking "+d.adress+";"+d.utPort);
				if (d.adress.equals(p.getAddress()) && d.utPort == p.getPort()) return d;
			}
			return null;
		}
	}
	private class RegisteredUser {
		String namn, losen;
		int poang;
		Dogfighter owner;
		public RegisteredUser(String n, String l, Dogfighter o) {
			namn = n;
			losen = l;
			owner = o;
		}
		public RegisteredUser(String n, String l, int p) {
			namn = n;
			losen = l;
			poang = p;
		}
		public String toString() {
			if (owner == null) return namn+"\n"+losen+"\n"+poang;
			return namn+"\n"+losen+"\n"+owner.poang;
		}
	}
}