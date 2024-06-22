//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Handles airdrops ramdomly called to specific location list ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class AirdropBase
{
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	protected ref array<AirDrop_Plane_Base> m_Planes;
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	static const int m_SecsPerMin = 60;		// Seconds to minutes conversion
	static const int m_Miliseconds = 1000;	// Seconds to miliseconds conversion
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	static float m_MapBX0 = 0.0;			// Map X border minimum
	static float m_MapBX1 = 15360.0;		// Map X border maximum
	static float m_MapBZ0 = 0.0;			// Map Z border minimum
	static float m_MapBZ1 = 15360.0;		// Map Z border maximum
	static float m_MapOX0 = -1000.0;		// Map X border offset for spawning/despawning aircraft
	static float m_MapOX1 = 16360.0;		// Map X border offset for spawning/despawning aircraft
	static float m_MapOZ0 = -1000.0;		// Map Z border offset for spawning/despawning aircraft
	static float m_MapOZ1 = 16360.0;		// Map Z border offset for spawning/despawning aircraft
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool m_RandomSpawn = false;				// Flag to manage display of notifications
	bool m_InProximity = false;				// Flag to indicate aircraft is with proximity of drop location and if notification has been displayed
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	vector m_DropPoint;						// Calculated position of drop point
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	ref Timer m_SpawnTimer;					// Timer controlling creation of random spawns
	ref AirdropSafezones m_Safezones;		// Airdrop safe zones loaded from JSON file
	ref AirdropSettings m_Settings;			// Airdrop settings loaded from JSON file
	ref AD_DropZone m_DropZone;				// Selected dropzone information
	int m_LastDropzone;						// The last dropzone selected (to prevent duplicat drops)
	int m_MessageColor;						// 
	string m_TxtMessage;					// 
	PlayerBase m_Player;					// 
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	FileHandle m_LogFile;					// Reference to log file
	FileHandle m_ExportFile;				// Reference to DropZone export file
	string m_LogMessage; 					// Message to be logged (temp)
	string m_Cardinal;						// Carinal heading
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void AirdropBase()
	{
		// Get randomization seed bsaed on game time
		//int SpawnSeed = ADU_GetFileDateTimeStamp(false).ToInt();
		//int SpawnSeed = Mathf.Math.RandomInt(−2147483648, 2147483647);
		// Set randomization seed
		//Math.Randomize(SpawnSeed);
		// Create array of aircraft as they are spawned
		m_Planes = new array<AirDrop_Plane_Base>;
		// Load Airdrop Settings
		m_Safezones = AirdropSafezones.Load();
		m_Settings = AirdropSettings.Load();
		// Add message color
		m_MessageColor = ARGB(m_Settings.Messages.NotificationARGB[0], m_Settings.Messages.NotificationARGB[1], m_Settings.Messages.NotificationARGB[2], m_Settings.Messages.NotificationARGB[3]);
		// Create log file for the aircraft manager
		string datetimestamp = AirdropBase.ADU_GetFileDateTimeStamp();
		if (m_Settings.Controls.AD_LogManager != 0) {
			m_LogFile = OpenFile(TEXT_PATH_LOGS_AIRDROP + datetimestamp + " AirdropManager.log", FileMode.WRITE);
		}
		// Clean up ALL old logs
		ADU_ClearOldAirdropFiles();
		// Calculate map dimentions and offsets
		ADU_SetMapProperties();
		// Get Random Drop Location
		m_DropZone = m_Settings.DropZones.GetRandomElement();
		// Setup Aircraft Spawn Timer (minutes)
		m_SpawnTimer = new Timer();
		m_SpawnTimer.Run(m_Settings.Controls.Interval * m_SecsPerMin, this, "EOnCallAirdrop", new Param2<bool, vector>(false, vector.Zero), true);
		// Export DropZones
		ADU_ExportDropZones();
		// Log activity
		ADU_LogMessage("[AirdropBase] Base class created & settings loaded.");
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void ~AirdropBase()
	{
		/// do whatever
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void EOnCallAirdrop(bool playercalled, vector droppoint, PlayerBase player)
	{
		//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// If 'Random Only' mode & 'Player Called'  -or- 'Player Only' mode and 'Random Called'
		if ((playercalled && m_Settings.Controls.Mode == 1) || (!playercalled && m_Settings.Controls.Mode == 2)) {
			// Disregard airdrop call
			return;
		}
		//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// If there are no players on the server 
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		int PlayerCount = players.Count();
		// If a random drop and there are NOT enough players the server
		if (!playercalled && PlayerCount < m_Settings.Controls.MinimumPlayers) {
			// Don't call an airdrop
			return;
		}
		//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Initialize log file header for this Airdrop call
		ADU_LogMessage("[" + AirdropBase.ADU_GetFileDateTimeStamp() + "] ----------------------------------------------------------------------------------------------------");
		//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// How is the airdrop being called 
		if (playercalled) {
			// Player called - location of player dropping flare
			m_Player = player;
			m_RandomSpawn = false;
			// Select 'Random' location properties
			m_DropZone = m_Settings.DropZones[0];
			// Set drop lcoation values within drop location
			m_DropZone.X = droppoint[0];
			m_DropZone.Z = droppoint[2];
			m_LastDropzone = 0;
		}
		else {
			// Randomly called - Select a random drop location
			m_Player = null;
			m_RandomSpawn = true;
			// Select random drop location (excluding 'Random' and last/previous drop location)
			int dzix = Math.RandomInt(1, m_Settings.DropZones.Count());
			while (m_LastDropzone == dzix) {
				dzix = Math.RandomInt(1, m_Settings.DropZones.Count());
			}
			m_DropZone = m_Settings.DropZones[dzix];
			droppoint = Vector(m_DropZone.X, 0, m_DropZone.Z);
			// Store dropzone name for later use
			m_LastDropzone = dzix;
		}
		//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		ADU_LogMessage("[EOnCallAirdrop] Drop zone asigned: " + m_DropZone.Title + " " + droppoint);
		// Set drop spawn location
		m_DropPoint = Vector(m_DropZone.X, m_Settings.Aircraft.StartAltMSL, m_DropZone.Z);
		// If the drop accuracy s greater than zero...		
		if (m_DropZone.DropAccuracy > 0) {
			// Calculate random drop location offset from preset location
			m_DropPoint = ADU_RandomWithinCircle(m_DropPoint, 0, m_DropZone.DropAccuracy, false, 0);
		}
		ADU_LogMessage("[EOnCallAirdrop] Drop zone offset to: " + m_DropPoint);
		//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Set Aircraft spawn location
		int side = Math.RandomIntInclusive(0, 3);
		float positionX = Math.RandomFloat(m_MapOX0, m_MapOX1);
		float positionZ = Math.RandomFloat(m_MapOZ0, m_MapOZ1);
		vector m_AircraftSpawn;
		switch (side) {
		case 0: {
			// North Edge
			m_AircraftSpawn[0] = positionX;
			m_AircraftSpawn[2] = m_MapOX1;
			break;
		}
		case 1: {
			// East Edge
			m_AircraftSpawn[0] = m_MapOZ1;
			m_AircraftSpawn[2] = positionZ;
			break;
		}
		case 2: {
			// South Edge
			m_AircraftSpawn[0] = positionX;
			m_AircraftSpawn[2] = m_MapOZ0;
			break;
		}
		case 3: {
			// West Edge
			m_AircraftSpawn[0] = m_MapOX0;
			m_AircraftSpawn[2] = positionZ;
			break;
		}
		}
		// Set aircraft's starting altitude
		m_AircraftSpawn[1] = m_Settings.Aircraft.StartAltMSL;
		ADU_LogMessage("[EOnCallAirdrop] Aircraft spawn point: " + m_AircraftSpawn);
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Create airdrop and set initial position  **TODO: Move object class name "AirdropPlane" to be spawned to config(zedmag)
		AirDrop_Plane_Base m_DropPlane = AirDrop_Plane_Base.Cast(GetGame().CreateObject("Land_AN_12", m_AircraftSpawn, false, false, true));
		// If we have an aircraft...
		if (m_DropPlane)
		{
			//Log type of plane (zedmag)
			string z_planeType = m_DropPlane.GetType();
			ADU_LogMessage("[EOnCallAirdrop] Aircraft type used=<" + z_planeType + ">");
			// Add aircraft to array
			m_Planes.Insert(m_DropPlane);
			// Setup aircraft
			m_DropPlane.SetPosition(m_AircraftSpawn);
			m_DropPlane.SetDirection(m_AircraftSpawn - m_DropPoint);
			// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			// Calculate angle to dropzone 
			float m_Angle = Math.Atan2(m_AircraftSpawn[2] - m_DropZone.Z, m_AircraftSpawn[0] - m_DropZone.X);
			float m_Bearing;
			// Calculate Cardinal Heading
			string cardinal = ADU_CalcCardinalHeading(m_Angle, m_Bearing);
			// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			// Send details to newly created aircraft
			m_DropPlane.InitializeAircraft(m_Settings, m_DropZone, m_RandomSpawn, m_Player, m_Cardinal);
			ADU_LogMessage("[EOnCallAirdrop] Aircraft bearing ... Angle/Bearing: " + m_Angle + "° / " + m_Bearing + "°, Text: '" + cardinal + "'");
			// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			// Send message indication aircraft is active
			if (m_RandomSpawn) {
				m_LogMessage = "Aircraft was dispatched to '" + m_DropZone.Title + "' (" + m_AircraftSpawn[0] + "/" + m_AircraftSpawn[2] + "), from the " + cardinal + ".";
				m_TxtMessage = "Aircraft was dispatched to '" + m_DropZone.Title + "', from the " + cardinal + ".";
				ADU_SendClientMessages(AIRDROP_ICON_AIRDROP_ACFT, m_TxtMessage, m_Settings.Messages.Mode, m_Settings.Messages.Duration, m_MessageColor);
			}
			else {
				m_LogMessage = "Aircraft was dispatched to the player's location (" + m_AircraftSpawn[0] + "/" + m_AircraftSpawn[2] + "), from the " + cardinal + ".";
				m_TxtMessage = "Aircraft was dispatched to the player's location, from the " + cardinal + ".";
				ADU_SendClientMessagesPrivate(AIRDROP_ICON_AIRDROP_ACFT, m_TxtMessage, m_Settings.Messages.Mode, m_Settings.Messages.Duration, m_MessageColor, m_Player);
			}
			ADU_LogMessage("[EOnCallAirdrop] " + m_LogMessage);
			// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		}
		else {
			ADU_LogMessage("[EOnCallAirdrop] Aircraft was called but was NOT created!");
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void ADU_LogMessage(string message) {
		// If we have a file and logs are requested... 
		if (m_LogFile && m_Settings.Controls.AD_LogManager != 0) {
			// Write message
			FPrintln(m_LogFile, message);
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	private void ADU_ClearOldAirdropFiles()
	{
		//······························································································································································································
		// Are we clearing old files (Max Backup days > 0)
		if (m_Settings.Controls.MaxBackupDays > 0) {
			ADU_LogMessage("[ClearOldAirdropFiles] Backups Cleanup Started.");
			// Initialize array of backup files 
			array<string> FileList = new array<string>;
			int FileAge;
			// Get list of backup Files
			FileList = ADU_FindFilesInLocation(JSON_PATH_SETTINGS_AIRDROP, "AirdropSettings_bakup_*.json");
			// Loop through the list of location types
			for (int ix = 0; ix < FileList.Count(); ix++) {
				FileAge = ADU_GetDateDiff(FileList[ix]);
				// If File is too Old 
				if (FileAge > m_Settings.Controls.MaxBackupDays) {
					// Delete File 
					DeleteFile(JSON_PATH_SETTINGS_AIRDROP + FileList[ix]);
					// Log activity
					ADU_LogMessage("[ClearOldAirdropFiles] Backup file removed:  " + FileList[ix] + " (" + FileAge + ")");
				}
			}
			ADU_LogMessage("[ClearOldAirdropFiles] Backups Cleanup Completed.");
		}
		else {
			ADU_LogMessage("[ClearOldAirdropFiles] Backup File Cleanup Disabled.");
		}
		//······························································································································································································
		// Are we clearing old files (Max Log days > 0)
		if (m_Settings.Controls.MaxLogDays > 0) {


			// Initialize array of log files 
			FileList = new array<string>;
			// Get list of backup Files
			FileList = ADU_FindFilesInLocation(TEXT_PATH_LOGS_AIRDROP, "*.log");
			// Process list of backup files 			
			// Loop through the list of location types
			for (int iz = 0; iz < FileList.Count(); iz++) {
				FileAge = ADU_GetDateDiff(FileList[iz]);
				// If File is too Old 
				if (FileAge > m_Settings.Controls.MaxLogDays) {
					// Delete File 
					DeleteFile(TEXT_PATH_LOGS_AIRDROP + FileList[iz]);
					// Log activity
					ADU_LogMessage("[ClearOldAirdropFiles] Airdrop Log File Rremoved:  " + FileList[iz] + " (" + FileAge + ")");
				}
			}
			ADU_LogMessage("[ClearOldAirdropFiles] Logs Cleanup Completed.");
		}
		else {
			ADU_LogMessage("[ClearOldAirdropFiles] Logs Cleanup Disabled.");
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	private int ADU_GetDateDiff(string FileName) {
		// Get current date string as integer ("21210102" = 20210127)
		int CurrentDate = AirdropBase.ADU_GetFileDateTimeStamp(false).Substring(0, 8).ToInt();
		// Cleanup String		
		// Backs:	AirdropSettings_bakup_2021-01-25 @ 04-53-51.json
		// Logs:	2021-01-25 @ 04-53-51 AirdropManager.log
		FileName.Replace("AirdropSettings_bakup_", "");
		FileName.Replace("-", "");
		// Get file date
		int FileDate = FileName.Substring(0, 8).ToInt();
		// return filelage in days
		return CurrentDate - FileDate;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	private void ADU_SetMapProperties() {
		m_MapBX0 = 0;									// Minimum map coordinates
		m_MapBX1 = m_Settings.Map.Height;				// Maximum map coordinates
		m_MapBZ0 = 0;									// Minimum map coordinates
		m_MapBZ1 = m_Settings.Map.Height;				// Maximum map coordinates
		m_MapOX0 = m_MapBX0 - m_Settings.Map.Offset;	// Minimum coordinates for despawning aircraft when off the map
		m_MapOX1 = m_MapBX1 + m_Settings.Map.Offset;	// Maximum coordinates for despawning aircraft when off the map
		m_MapOZ0 = m_MapBZ0 - m_Settings.Map.Offset;	// Minimum coordinates for despawning aircraft when off the map
		m_MapOZ1 = m_MapBZ1 + m_Settings.Map.Offset;	// Maximum coordinates for despawning aircraft when off the map
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	private string ADU_CalcCardinalHeading(float angle, out float bearing) {
		string cardinal;
		// calculate bearing from information provided
		bearing = 90.0 - (Math.RAD2DEG * angle);
		// Constrain bearing within 0° and 360°
		if (bearing < 0) {
			bearing = 360 + bearing;
		}
		if (bearing > 360) {
			bearing = bearing - 360;
		}
		// Select cardinal heading based on bearing°
		if (bearing >= -22.5 && bearing < 22.5) {
			cardinal = "North";
		}
		else if (bearing >= 22.5 && bearing < 67.5) {
			cardinal = "North/East";
		}
		else if (bearing >= 67.5 && bearing < 112.5) {
			cardinal = "East";
		}
		else if (bearing >= 112.5 && bearing < 157.5) {
			cardinal = "South/East";
		}
		else if (bearing >= 157.5 && bearing < 202.5) {
			cardinal = "South";
		}
		else if (bearing >= 202.5 && bearing < 247.5) {
			cardinal = "South/West";
		}
		else if (bearing >= 247.5 && bearing < 292.5) {
			cardinal = "West";
		}
		else if (bearing >= 292.5 && bearing < 337.5) {
			cardinal = "North/West";
		}
		else if (bearing >= 337.5 && bearing < 382.5) {
			cardinal = "North";
		}
		// 
		int brng = Math.AbsInt(Math.Round(bearing));
		string zeros = "";
		if (brng < 100) { zeros = "0"; }
		if (brng <  10) { zeros = "00"; }
		// Add bearing
		cardinal = cardinal + " (" + zeros.Trim() + brng.ToString().Trim() + "°)";
		// Return cardinal heading
		m_Cardinal  = cardinal;
		return cardinal;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void ADU_ReplaceAirdropFlare(PlayerBase player, vector PlayerPosition)
	{
		// If we have a player
		if (player) {
			// Current player player
			m_Player = player;
			// Fetch player's identity
			PlayerIdentity identity = player.GetIdentity();
			// Select random position about player
			vector m_SpawnPoint = AirdropBase.ADU_RandomWithinCircle(PlayerPosition, 0.2, 0.5, true, 0.01);
			// Spawn RoadFlare in hand
			EntityAI itemEnt = player.GetHumanInventory().CreateInHands("Roadflare");
			// If we have a Road Flare
			if (itemEnt) {
				// Activate it
				itemEnt.GetCompEM().SwitchOn();
				// If we have a player's identity
				if (identity) {
					// Player's 'Road Flare' activated
					ADU_LogMessage("[ReplaceAirdropFlare] 'Road Flare' activted in player's hand (" + identity.GetName() + ").");
					// Call an Airdrop to player's position
					EOnCallAirdrop(true, PlayerPosition, player);
				}
			}
			else {
				// We didn't get a handle to the item
				ADU_LogMessage("[ReplaceAirdropFlare] Couldn't get handle to 'Road Flare' in player's hand (" + identity.GetName() + ").");
			}
		}
		else {
			// We didn't get a handle to the player
			Print("[ReplaceAirdropFlare] Wasn't passed handle to player!");
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	string ADU_WithinSafezone(vector position) {
		// Within m_Safezone flag
		bool m_WithinZone = false;
		string m_SZmessage = "";
		float m_SZDist = 0;
		vector m_SZposition;
		// Loop through all safezones and check if position iw within safezone
		for (int s0 = 0; s0 < m_Safezones.SafeZones.Count(); s0++) {
			// Get temp handle to safezone and create vector of position
			AD_SafeZones safezone = m_Safezones.SafeZones[s0];
			m_SZposition = Vector(safezone.X, 0, safezone.Z);
			// Calculate distance between player and safezone
			m_SZDist = ADU_CalcDistance(position, m_SZposition);
			// Are we within the safezone radius?
			if (m_SZDist <= safezone.Radius) {
				m_WithinZone = true;
				m_SZmessage = safezone.Message;
			}
		}
		// Return state
		return m_SZmessage;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void ADU_ExportDropZones() {
		// If we are exposting DropZones...
		if (m_Settings.VPP_Map.ExportMap != 0) {
			// Open the export file (replacing content)
			m_ExportFile = OpenFile(JSON_FILE_EXPORT_DROPZONES, FileMode.WRITE);
			string m_WriteLine = "";
			// For each DropZone... 
			int t_Dropzones = m_Settings.DropZones.Count();
			// We have exported the dorpzone data
			ADU_LogMessage("[ExportDropZones] Exporting " + t_Dropzones + " DropZone coordinates.");
			for (int ix = 1; ix < t_Dropzones; ix++)
			{
				m_WriteLine += "        {#";
				// Are we doing Titles (0) or Numbers (1)?
				if (m_Settings.VPP_Map.TitleMode != 0) {
					m_WriteLine += "            |M_MARKER_NAME|:|" + m_Settings.DropZones[ix].Title.Trim() + "|,#";
				} else {
					m_WriteLine += "            |M_MARKER_NAME|:|AD-" + ix.ToString().Trim() + "|,#";
				}
			  	m_WriteLine += "            |M_ICON_PATH|:|VanillaPPMap^^GUI^^Textures^^CustomMapIcons^^" + m_Settings.VPP_Map.MapIcon.Trim() + "|,#";
			  	m_WriteLine += "            |M_COLOR|: [ " + m_Settings.VPP_Map.MapColor[0] + ", " + m_Settings.VPP_Map.MapColor[1] + ", " + m_Settings.VPP_Map.MapColor[2] + " ],#";
				m_WriteLine += "            |M_POSITION|: [ " + Math.Round(m_Settings.DropZones[ix].X) + ", 0, " + Math.Round(m_Settings.DropZones[ix].Z) + " ],#";
				m_WriteLine += "            |M_ISACTIVE|: " + m_Settings.VPP_Map.IsActive + ",#";
				m_WriteLine += "            |M_IS_3D_ACTIVE|: " + m_Settings.VPP_Map.Is3DActive + "#";
				// Close out this item
				if (ix < t_Dropzones - 1) {
					m_WriteLine += "        },#";
				} 
				else {
					m_WriteLine += "        }";
				}
			}		
			// Replace setup
			int t_Int = 34;
			string t_Quote = t_Int.AsciiToString();
			t_Int = 10;
			string t_CR = t_Int.AsciiToString();
			// Replace pipes (|) with dbl-quotes (")
			m_WriteLine.Replace("|", t_Quote);
			// Replace pounds (#) with carrage return
			m_WriteLine.Replace("#", t_CR);
			// Replace carrots (^) with double-slashes (\\)
			m_WriteLine.Replace("^", "\\");
			// Write the text to file
			FPrintln(m_ExportFile, m_WriteLine);
			// Close export file
			CloseFile(m_ExportFile);
			// We have exported the dorpzone data
			ADU_LogMessage("[ExportDropZones] DropZone coordinates exported to VPP Map file.");
		}
		else {
			// We are not exporting dorpzone data
			ADU_LogMessage("[ExportDropZones] DropZone exported not bypassed.");
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	static vector ADU_RandomWithinCircle(vector centerpoint, float mindistance, float maxdistance, bool grounded, float groundoffset) {
		// Calculate random heading (degrees)
		float h = Math.RandomFloat(0.0, 2.0) * Math.PI;
		// Calculate random distance (meters)
		float d = Math.RandomFloat(mindistance, maxdistance);
		// Add offset to center point (vector)
		vector offsetposition = centerpoint + Vector(Math.Cos(h) * d, 0.0, Math.Sin(h) * d);
		// Does the new position need to be grounded
		if (grounded) {
			offsetposition[1] = GetGame().SurfaceY(offsetposition[0], offsetposition[2]) + groundoffset;
		}
		// Return offset position
		return offsetposition;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//! Check if aircraft is still on map
	static bool ADU_IsInRect(vector position) {
		// Check if object is on the map, based on defined map height, width, and offset parameters
		if ((position[0] >= m_MapOX0) && (position[0] <= m_MapOX1) && (position[2] >= m_MapOZ0) && (position[2] <= m_MapOZ1)) {
			return true;
		}
		else {
			return false;
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	static void ADU_SendClientMessages(string icon, string message, int messagemode, int duration, int messagecolor) {
		// If icon is null then use default
		if (!icon) {
			icon = AIRDROP_ICON_AIRDROP_ACFT;
		}
		// If messages are NOT turned off (modes 1, 2, & 3) 
		if (messagemode > 0 && messagemode <= 3) {
			// If Message Mode is set to Form or Both ... m_Settings.Messages.Mode 
			if ((messagemode == 1) || (messagemode == 3)) {
				NotificationSystem.SendNotificationToPlayerIdentityExtended(null, duration, "Airdrop...", message, icon);
			}
			// If Message Mode is set to Text or Both
			if ((messagemode == 2) || (messagemode == 3)) {
				ADU_SendChatMessage("Airdrop:  " + message);
			}
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	static void ADU_SendChatMessage(string message)
	{
		ref array<Man> players = new array<Man>;
		// Get array of active players
		GetGame().GetPlayers(players);
		// Build message parameter
		Param1<string> m_MessageParam = new Param1<string>(message);
		// Loop through player array
		foreach(auto player : players)
		{
			// Send player text message
			GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, m_MessageParam, true, player.GetIdentity());
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	static void ADU_SendClientMessagesPrivate(string icon, string message, int messagemode, int duration, int messagecolor, PlayerBase player) {
		// If icon is null then use default
		if (!icon) {
			icon = AIRDROP_ICON_AIRDROP_ACFT;
		}
		// If messages are NOT turned off (modes 1, 2, & 3) 
		if (messagemode > 0 && messagemode <= 3) {
			// If Message Mode is set to Form or Both ... m_Settings.Messages.Mode 
			if ((messagemode == 1) || (messagemode == 3)) {
				NotificationSystem.SendNotificationToPlayerExtended(player, duration, "Airdrop...", message, icon);
			}
			// If Message Mode is set to Text or Both
			if ((messagemode == 2) || (messagemode == 3)) {
				ADU_SendPrivateChatMessage("Airdrop:  " + message, player);
			}
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	static void ADU_SendPrivateChatMessage(string message, PlayerBase player)
	{
		// Send player tect message
		Param1<string> m_MessageParam = new Param1<string>(message);
		GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, m_MessageParam, true, player.GetIdentity());
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	static string ADU_GetFileDateTimeStamp(bool Formated = true) {
		int year, month, day, hour, minute, second;
		string y, m, d, h, n, s, timeStamp;
		// Get current server date & time
		GetHourMinuteSecondUTC(hour, minute, second);
		GetYearMonthDayUTC(year, month, day);
		// Convert ints to string
		y = year.ToString().Trim();
		m = month.ToString().Trim();
		d = day.ToString().Trim();
		h = hour.ToString().Trim();
		n = minute.ToString().Trim();
		s = second.ToString().Trim();
		// If only 1 character then add leading 0
		if (m.Length() == 1) { m = "0" + m; }
		if (d.Length() == 1) { d = "0" + d; }
		if (h.Length() == 1) { h = "0" + h; }
		if (n.Length() == 1) { n = "0" + n; }
		if (s.Length() == 1) { s = "0" + s; }
		// Build date time string - Reverse date provides proper file sorting
		if (Formated) {
			// 2019-01-01 @ 12-47-35
			timeStamp = string.Format("%1-%2-%3 @ %4-%5-%6", y, m, d, h, n, s);
		}
		else {
			// 20190101124735
			timeStamp = string.Format("%1%2%3%4%5%6", y, m, d, h, n, s);
		}
		return timeStamp;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	static float ADU_CalcDistance(vector position1, vector position2) {
		// Calculate distance to release 
		return Math.Sqrt(Math.Pow(position1[0] - position2[0], 2) + Math.Pow(position1[2] - position2[2], 2));
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	static array< string > ADU_FindFilesInLocation(string folder, string filter) {
		// Initialize array of filenames and filename var
		array<string> files = new array<string>;
		string fileName;
		// 
		FileAttr fileAttr;
		// Fetch the first file
		FindFileHandle findFileHandle = FindFile(folder + filter, fileName, fileAttr, 0);
		// If we got a handle to a file
		if (findFileHandle) {
			// If we have a filename and it is not a directory...
			if (fileName.Length() > 0 && !(fileAttr & FileAttr.DIRECTORY)) {
				// Add the filename to the list
				files.Insert(fileName);
			}
			// Fetch the remaining files
			while (FindNextFile(findFileHandle, fileName, fileAttr)) {
				// If we have a filename and it is not a directory...
				if (fileName.Length() > 0 && !(fileAttr & FileAttr.DIRECTORY)) {
					// Add the filename to the list
					files.Insert(fileName);
				}
			}
		}
		// Cleanup file handler
		CloseFindFile(findFileHandle);
		return files;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static ref AirdropBase g_AirdropBase;
static ref AirdropBase GetAirdropBase()
{
	if (!g_AirdropBase)
	{
		g_AirdropBase = new ref AirdropBase();
	}
	return g_AirdropBase;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
