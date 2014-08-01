using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;

namespace SMPSCFG
{
	public partial class MainForm : Form
	{
		static MainForm Instance;

		public MainForm()
		{
			InitializeComponent();
			Instance = this;
		}

		static readonly string[] TrackKeys = {
			"TitleScreenTrack",
			"AngelIsland1Track",
			"AngelIsland2Track",
			"Hydrocity1Track",
			"Hydrocity2Track",
			"MarbleGarden1Track",
			"MarbleGarden2Track",
			"CarnivalNight1Track",
			"CarnivalNight2Track",
			"FlyingBattery1Track",
			"FlyingBattery2Track",
			"IceCap1Track",
			"IceCap2Track",
			"LaunchBase1Track",
			"LaunchBase2Track",
			"MushroomHill1Track",
			"MushroomHill2Track",
			"Sandopolis1Track",
			"Sandopolis2Track",
			"LavaReef1Track",
			"LavaReef2Track",
			"SkySanctuaryTrack",
			"DeathEgg1Track",
			"DeathEgg2Track",
			"MidbossTrack",
			"BossTrack",
			"DoomsdayTrack",
			"MagneticOrbsTrack",
			"SpecialStageTrack",
			"SlotMachineTrack",
			"GumballMachineTrack",
			"KnucklesTrack",
			"AzureLakeTrack",
			"BalloonParkTrack",
			"DesertPalaceTrack",
			"ChromeGadgetTrack",
			"EndlessMineTrack",
			"GameOverTrack",
			"ContinueTrack",
			"ActClearTrack",
			"1UpTrack",
			"ChaosEmeraldTrack",
			"InvincibilityTrack",
			"CompetitionMenuTrack",
			"UnusedTrack",
			"LevelSelectTrack",
			"FinalBossTrack",
			"DrowningTrack",
			"AllClearTrack",
			"CreditsTrack",
			"HiddenPalaceTrack"
		};

		static readonly string[] TrackNames = {
			"Title Screen",
			"Angel Island Zone Act 1",
			"Angel Island Zone Act 2",
			"Hydrocity Zone Act 1",
			"Hydrocity Zone Act 2",
			"Marble Garden Zone Act 1",
			"Marble Garden Zone Act 2",
			"Carnival Night Zone Act 1",
			"Carnival Night Zone Act 2",
			"Flying Battery Zone Act 1",
			"Flying Battery Zone Act 2",
			"IceCap Zone Act 1",
			"IceCap Zone Act 2",
			"Launch Base Zone Act 1",
			"Launch Base Zone Act 2",
			"Mushroom Hill Zone Act 1",
			"Mushroom Hill Zone Act 2",
			"Sandopolis Zone Act 1",
			"Sandopolis Zone Act 2",
			"Lava Reef Zone Act 1",
			"Lava Reef Zone Act 2",
			"Sky Sanctuary Zone",
			"Death Egg Zone Act 1",
			"Death Egg Zone Act 2",
			"Midboss",
			"Boss",
			"The Doomsday Zone",
			"Magnetic Orbs",
			"Special Stage",
			"Slot Machine",
			"Gumball Machine",
			"Knuckles' Theme",
			"Azure Lake",
			"Balloon Park",
			"Desert Palace",
			"Chrome Gadget",
			"Endless Mine",
			"Game Over",
			"Continue",
			"Act Clear",
			"1-Up",
			"Chaos Emerald",
			"Invincibility",
			"Competition Menu",
			"Unused",
			"Level Select",
			"Final Boss",
			"Drowning",
			"All Clear",
			"Credits",
			"Hidden Palace Zone"
		};

		enum MusicID : byte
		{
			S3Title,
			AngelIsland1,
			AngelIsland2,
			Hydrocity1,
			Hydrocity2,
			MarbleGarden1,
			MarbleGarden2,
			CarnivalNight1,
			CarnivalNight2,
			FlyingBattery1,
			FlyingBattery2,
			IceCap1,
			IceCap2,
			LaunchBase1,
			LaunchBase2,
			MushroomHill1,
			MushroomHill2,
			Sandopolis1,
			Sandopolis2,
			LavaReef1,
			LavaReef2,
			SkySanctuary,
			DeathEgg1,
			DeathEgg2,
			Midboss,
			SKMidboss = Midboss,
			Boss,
			Doomsday,
			MagneticOrbs,
			SpecialStage,
			SlotMachine,
			GumballMachine,
			S3Knuckles,
			AzureLake,
			BalloonPark,
			DesertPalace,
			ChromeGadget,
			EndlessMine,
			GameOver,
			Continue,
			ActClear,
			S31Up,
			ChaosEmerald,
			S3Invincibility,
			CompetitionMenu,
			Unused,
			S3Midboss = Unused,
			LevelSelect,
			FinalBoss,
			Drowning,
			S3AllClear,
			S3Credits,
			SKKnuckles,
			SKTitle,
			SK1Up,
			SKInvincibility,
			SKAllClear,
			SKCredits,
			S3CCredits,
			HiddenPalace = S3CCredits,
			S3Continue,
			// begin S&KC tracks
			CarnivalNight1PC,
			CarnivalNight2PC,
			IceCap1PC,
			IceCap2PC,
			LaunchBase1PC,
			LaunchBase2PC,
			KnucklesPC,
			CompetitionMenuPC,
			UnusedPC,
			CreditsPC,
			S3InvincibilityPC,
			// end S&KC tracks
			SKTitle0525,
			SKAllClear0525,
			SKCredits0525,
			GreenGrove1,
			GreenGrove2,
			RustyRuin1,
			RustyRuin2,
			VolcanoValley2,
			VolcanoValley1,
			SpringStadium1,
			SpringStadium2,
			DiamondDust1,
			DiamondDust2,
			GeneGadget1,
			GeneGadget2,
			PanicPuppet2,
			FinalFight,
			S3DEnding,
			S3DSpecialStage,
			PanicPuppet1,
			S3DBoss2,
			S3DBoss1,
			S3DIntro,
			S3DCredits,
			S3DInvincibility,
			S3DMenu,
			S4E1Boss
		}

		static readonly Dictionary<string, MusicID> TitleScreenTrackOptions = new Dictionary<string, MusicID>() {
			{ "S3", MusicID.S3Title },
			{ "S&K", MusicID.SKTitle },
			{ "S&K0525", MusicID.SKTitle0525 },
			{ "S3D", MusicID.S3DIntro }
		};

		static readonly Dictionary<string, MusicID> AngelIsland1TrackOptions = new Dictionary<string, MusicID>() {
			{ "S3K", MusicID.AngelIsland1 },
			{ "GreenGrove", MusicID.GreenGrove1 }
		};

		static readonly Dictionary<string, MusicID> AngelIsland2TrackOptions = new Dictionary<string, MusicID>() {
			{ "S3K", MusicID.AngelIsland2 },
			{ "GreenGrove", MusicID.GreenGrove2 }
		};

		static readonly Dictionary<string, MusicID> Hydrocity1TrackOptions = new Dictionary<string, MusicID>() {
			{ "S3K", MusicID.Hydrocity1 },
			{ "RustyRuin", MusicID.RustyRuin1 },
		};

		static readonly Dictionary<string, MusicID> Hydrocity2TrackOptions = new Dictionary<string, MusicID>() {
			{ "S3K", MusicID.Hydrocity2 },
			{ "RustyRuin", MusicID.RustyRuin2 }
		};

		static readonly Dictionary<string, MusicID> CarnivalNight1TrackOptions = new Dictionary<string, MusicID>() {
			{ "MD", MusicID.CarnivalNight1 },
			{ "PC", MusicID.CarnivalNight1PC },
			{ "SpringStadium", MusicID.SpringStadium1 }
		};

		static readonly Dictionary<string, MusicID> CarnivalNight2TrackOptions = new Dictionary<string, MusicID>() {
			{ "MD", MusicID.CarnivalNight2 },
			{ "PC", MusicID.CarnivalNight2PC },
			{ "SpringStadium", MusicID.SpringStadium2 }
		};

		static readonly Dictionary<string, MusicID> FlyingBattery1TrackOptions = new Dictionary<string, MusicID>() {
			{ "S3K", MusicID.FlyingBattery1 },
			{ "PanicPuppet", MusicID.PanicPuppet1 }
		};

		static readonly Dictionary<string, MusicID> FlyingBattery2TrackOptions = new Dictionary<string, MusicID>() {
			{ "S3K", MusicID.FlyingBattery2 },
			{ "PanicPuppet", MusicID.PanicPuppet2 }
		};

		static readonly Dictionary<string, MusicID> IceCap1TrackOptions = new Dictionary<string, MusicID>() {
			{ "MD", MusicID.IceCap1 },
			{ "PC", MusicID.IceCap1PC },
			{ "DiamondDust", MusicID.DiamondDust1 }
		};

		static readonly Dictionary<string, MusicID> IceCap2TrackOptions = new Dictionary<string, MusicID>() {
			{ "MD", MusicID.IceCap2 },
			{ "PC", MusicID.IceCap2PC },
			{ "DiamondDust", MusicID.DiamondDust2 }
		};

		static readonly Dictionary<string, MusicID> LaunchBase1TrackOptions = new Dictionary<string, MusicID>() {
			{ "MD", MusicID.LaunchBase1 },
			{ "PC", MusicID.LaunchBase1PC },
			{ "GeneGadget", MusicID.GeneGadget1 },
			{ "PanicPuppet", MusicID.PanicPuppet1 }
		};

		static readonly Dictionary<string, MusicID> LaunchBase2TrackOptions = new Dictionary<string, MusicID>() {
			{ "MD", MusicID.LaunchBase2 },
			{ "PC", MusicID.LaunchBase2PC },
			{ "GeneGadget", MusicID.GeneGadget2 },
			{ "PanicPuppet", MusicID.PanicPuppet2 }
		};

		static readonly Dictionary<string, MusicID> MushroomHill1TrackOptions = new Dictionary<string, MusicID>() {
			{ "S3K", MusicID.MushroomHill1 },
			{ "GreenGrove", MusicID.GreenGrove1 }
		};

		static readonly Dictionary<string, MusicID> MushroomHill2TrackOptions = new Dictionary<string, MusicID>() {
			{ "S3K", MusicID.MushroomHill2 },
			{ "GreenGrove", MusicID.GreenGrove2 }
		};

		static readonly Dictionary<string, MusicID> LavaReef1TrackOptions = new Dictionary<string, MusicID>() {
			{ "S3K", MusicID.LavaReef1 },
			{ "VolcanoValley", MusicID.VolcanoValley1 }
		};

		static readonly Dictionary<string, MusicID> LavaReef2TrackOptions = new Dictionary<string, MusicID>() {
			{ "S3K", MusicID.LavaReef2 },
			{ "VolcanoValley", MusicID.VolcanoValley2 }
		};

		static readonly Dictionary<string, MusicID> DeathEgg1TrackOptions = new Dictionary<string, MusicID>() {
			{ "S3K", MusicID.DeathEgg1 },
			{ "GeneGadget", MusicID.GeneGadget1 },
			{ "PanicPuppet", MusicID.PanicPuppet1 }
		};

		static readonly Dictionary<string, MusicID> DeathEgg2TrackOptions = new Dictionary<string, MusicID>() {
			{ "S3K", MusicID.DeathEgg2 },
			{ "GeneGadget", MusicID.GeneGadget2 },
			{ "PanicPuppet", MusicID.PanicPuppet2 }
		};

		static readonly Dictionary<string, MusicID> MidbossTrackOptions = new Dictionary<string, MusicID>() {
			{ "S3", MusicID.S3Midboss },
			{ "S&K", MusicID.SKMidboss },
			{ "S3D1", MusicID.S3DBoss1 },
			{ "S3D2", MusicID.S3DBoss2 },
			{ "S4E1", MusicID.S4E1Boss }
		};

		static readonly Dictionary<string, MusicID> BossTrackOptions = new Dictionary<string, MusicID>() {
			{ "S3K", MusicID.Boss },
			{ "S3D1", MusicID.S3DBoss1 },
			{ "S3D2", MusicID.S3DBoss2 },
			{ "S4E1", MusicID.S4E1Boss }
		};

		static readonly Dictionary<string, MusicID> DoomsdayTrackOptions = new Dictionary<string, MusicID>() {
			{ "S3K", MusicID.Doomsday },
			{ "S3D1", MusicID.S3DBoss1 },
			{ "S3D2", MusicID.S3DBoss2 },
			{ "FinalFight", MusicID.FinalFight },
			{ "S4E1", MusicID.S4E1Boss }
		};

		static readonly Dictionary<string, MusicID> SpecialStageTrackOptions = new Dictionary<string, MusicID>() {
			{ "S3K", MusicID.SpecialStage },
			{ "S3D", MusicID.S3DSpecialStage }
		};

		static readonly Dictionary<string, MusicID> KnucklesTrackOptions = new Dictionary<string, MusicID>() {
			{ "S3", MusicID.S3Knuckles },
			{ "S&K", MusicID.SKKnuckles },
			{ "PC", MusicID.KnucklesPC }
		};

		static readonly Dictionary<string, MusicID> ContinueTrackOptions = new Dictionary<string, MusicID>() { { "S3", MusicID.S3Continue }, { "S&K", MusicID.Continue } };

		static readonly Dictionary<string, MusicID> OneUpTrackOptions = new Dictionary<string, MusicID>() { { "S3", MusicID.S31Up }, { "S&K", MusicID.SK1Up } };

		static readonly Dictionary<string, MusicID> InvincibilityTrackOptions = new Dictionary<string, MusicID>() {
			{ "S3", MusicID.S3Invincibility },
			{ "S&K", MusicID.SKInvincibility },
			{ "S3PC", MusicID.S3InvincibilityPC },
			{ "PC", MusicID.UnusedPC },
			{ "S3D", MusicID.S3DInvincibility }
		};

		static readonly Dictionary<string, MusicID> CompetitionMenuTrackOptions = new Dictionary<string, MusicID>() {
			{ "MD", MusicID.CompetitionMenu },
			{ "PC", MusicID.CompetitionMenuPC },
			{ "S3D", MusicID.S3DMenu }
		};

		static readonly Dictionary<string, MusicID> LevelSelectTrackOptions = new Dictionary<string, MusicID>() {
			{ "S3K", MusicID.LevelSelect },
			{ "S3D", MusicID.S3DMenu },
			{ "PanicPuppet1", MusicID.PanicPuppet1 }
		};

		static readonly Dictionary<string, MusicID> FinalBossTrackOptions = new Dictionary<string, MusicID>() {
			{ "S3K", MusicID.FinalBoss },
			{ "S3D1", MusicID.S3DBoss1 },
			{ "S3D2", MusicID.S3DBoss2 },
			{ "FinalFight", MusicID.FinalFight },
			{ "S4E1", MusicID.S4E1Boss }
		};

		static readonly Dictionary<string, MusicID> AllClearTrackOptions = new Dictionary<string, MusicID>() {
			{ "S3", MusicID.S3AllClear },
			{ "S&K", MusicID.SKAllClear },
			{ "S&K0525", MusicID.SKAllClear0525 },
			{ "S3D", MusicID.S3DEnding }
		};

		static readonly Dictionary<string, MusicID> CreditsTrackOptions = new Dictionary<string, MusicID>() {
			{ "S3", MusicID.S3Credits },
			{ "S&K", MusicID.SKCredits },
			{ "S3C", MusicID.S3CCredits },
			{ "S&K0525", MusicID.SKCredits0525 },
			{ "PC", MusicID.CreditsPC },
			{ "S3D", MusicID.S3DCredits }
		};

		static readonly Dictionary<string, MusicID>[] TrackOptions = {
			TitleScreenTrackOptions,
			AngelIsland1TrackOptions,
			AngelIsland2TrackOptions,
			Hydrocity1TrackOptions,
			Hydrocity2TrackOptions,
			null,
			null,
			CarnivalNight1TrackOptions,
			CarnivalNight2TrackOptions,
			FlyingBattery1TrackOptions,
			FlyingBattery2TrackOptions,
			IceCap1TrackOptions,
			IceCap2TrackOptions,
			LaunchBase1TrackOptions,
			LaunchBase2TrackOptions,
			MushroomHill1TrackOptions,
			MushroomHill2TrackOptions,
			null,
			null,
			LavaReef1TrackOptions,
			LavaReef2TrackOptions,
			null,
			DeathEgg1TrackOptions,
			DeathEgg2TrackOptions,
			MidbossTrackOptions,
			BossTrackOptions,
			DoomsdayTrackOptions,
			null,
			SpecialStageTrackOptions,
			null,
			null,
			KnucklesTrackOptions,
			null,
			null,
			null,
			null,
			null,
			null,
			ContinueTrackOptions,
			null,
			OneUpTrackOptions,
			null,
			InvincibilityTrackOptions,
			CompetitionMenuTrackOptions,
			null,
			LevelSelectTrackOptions,
			FinalBossTrackOptions,
			null,
			AllClearTrackOptions,
			CreditsTrackOptions,
			null
		};

		static readonly string[] GameIDs = { string.Empty, "S3K", "S3", "S&K" };

		Dictionary<string, Dictionary<string, string>> INI;

		static readonly List<ComboBox>[] comboboxes = new List<ComboBox>[4];

		#region Native Methods
		[DllImport("kernel32", SetLastError = true, CharSet = CharSet.Auto)]
		static extern IntPtr LoadLibrary(string lpFileName);
		[DllImport("kernel32.dll", SetLastError = true)]
		static extern bool FreeLibrary(IntPtr hModule);
		[DllImport("kernel32", CharSet = CharSet.Ansi, ExactSpelling = true, SetLastError = true)]
		static extern IntPtr GetProcAddress(IntPtr hModule, string procName);
		#endregion

		#region Delegates
		delegate bool InitializeDriverDelegate();
		delegate bool PlaySongDelegate(byte song);
		delegate bool StopSongDelegate();
		delegate void RegisterSongStoppedCallbackDelegate([MarshalAs(UnmanagedType.FunctionPtr)] Action callback);
		#endregion

		IntPtr hModule;
		PlaySongDelegate PlaySong;
		StopSongDelegate StopSong;
		static Action SongStoppedCallback = SongStopped;

		int playingSongSelection = -1;
		Button playingButton = null;
		Random songPicker = new Random();

		private void MainForm_Load(object sender, EventArgs e)
		{
			TableLayoutPanel[] tables = { tableLayoutPanel1, tableLayoutPanel2, tableLayoutPanel3, tableLayoutPanel4 };
			if (File.Exists("SMPSOUT.ini"))
				INI = IniFile.IniFile.Load("SMPSOUT.ini");
			else
				INI = new Dictionary<string, Dictionary<string, string>>();
			IntPtr ptr;
			foreach (string file in Directory.GetFiles(Environment.CurrentDirectory, "*.dll"))
			{
				IntPtr handle = LoadLibrary("MIDIOUT.DLL");
				ptr = GetProcAddress(handle, "InitializeDriver");
				if (ptr == IntPtr.Zero)
					continue;
				hModule = handle;
				((InitializeDriverDelegate)Marshal.GetDelegateForFunctionPointer(ptr, typeof(InitializeDriverDelegate)))();
			}
			if (hModule != IntPtr.Zero)
			{
				ptr = GetProcAddress(hModule, "PlaySong");
				PlaySong = (PlaySongDelegate)Marshal.GetDelegateForFunctionPointer(ptr, typeof(PlaySongDelegate));
				ptr = GetProcAddress(hModule, "StopSong");
				StopSong = (StopSongDelegate)Marshal.GetDelegateForFunctionPointer(ptr, typeof(StopSongDelegate));
				ptr = GetProcAddress(hModule, "RegisterSongStoppedCallback");
				((RegisterSongStoppedCallbackDelegate)Marshal.GetDelegateForFunctionPointer(ptr, typeof(RegisterSongStoppedCallbackDelegate)))(SongStoppedCallback);
			}
			for (int gn = 0; gn < GameIDs.Length; gn++)
			{
				string game = GameIDs[gn];
				if (!INI.ContainsKey(game))
					INI.Add(game, new Dictionary<string, string>());
				Dictionary<string, string> group = INI[game];
				TableLayoutPanel table = tables[gn];
				table.SuspendLayout();
				comboboxes[gn] = new List<ComboBox>();
				for (int tn = 0; tn < TrackKeys.Length; tn++)
				{
					string track = TrackKeys[tn];
					table.Controls.Add(new Label() { Text = TrackNames[tn] + ":", AutoSize = true, TextAlign = ContentAlignment.MiddleLeft }, 0, tn);
					ComboBox cb = new ComboBox() { DropDownStyle = ComboBoxStyle.DropDownList };
					cb.Items.Add("Default");
					cb.Items.Add("MIDI");
					Dictionary<string, MusicID> opts = TrackOptions[tn];
					string[] optnames = opts != null ? opts.Keys.ToArray() : null;
					MusicID[] optids = opts != null ? opts.Values.ToArray() : null;
					if (opts != null)
					{
						cb.Items.Add("Random");
						cb.Items.AddRange(optnames);
					}
					table.Controls.Add(cb, 1, tn);
					comboboxes[gn].Add(cb);
					Button btn = hModule != IntPtr.Zero && opts != null ? new Button() { Enabled = false, Text = "Play" } : null;
					if (btn != null)
						table.Controls.Add(btn, 2, tn);
					if (!group.ContainsKey(track))
					{
						group.Add(track, "Default");
						cb.SelectedIndex = 0;
					}
					else
						switch ((group[track] ?? "default").ToLowerInvariant())
						{
							case "default":
								cb.SelectedIndex = 0;
								break;
							case "midi":
								cb.SelectedIndex = 1;
								break;
							case "random":
								cb.SelectedIndex = opts != null ? 2 : 0;
								if (btn != null)
									btn.Enabled = true;
								break;
							default:
								cb.SelectedIndex = 0;
								if (opts != null)
									for (int on = 0; on < optnames.Length; on++)
										if (group[track].Equals(optnames[on], StringComparison.OrdinalIgnoreCase))
										{
											cb.SelectedIndex = on + 3;
											if (btn != null)
												btn.Enabled = true;
											break;
										}
								break;
						}
					if (tn > 0)
						table.RowStyles.Add(new RowStyle(SizeType.AutoSize));
					if (opts == null)
						cb.SelectedIndexChanged += new EventHandler((s, ev) =>
						{
							string str;
							if (cb.SelectedIndex == 1)
								str = "MIDI";
							else
								str = "Default";
							INI[game][track] = str;
						});
					else
					{
						cb.SelectedIndexChanged += new EventHandler((s, ev) =>
						{
							string str;
							if (cb.SelectedIndex > 2)
								str = optnames[cb.SelectedIndex - 3];
							else if (cb.SelectedIndex == 2)
								str = "Random";
							else if (cb.SelectedIndex == 1)
								str = "MIDI";
							else
								str = "Default";
							INI[game][track] = str;
							if (btn != null)
							{
								btn.Enabled = cb.SelectedIndex > 1;
								btn.Text = playingButton == btn && playingSongSelection == cb.SelectedIndex ? "Stop" : "Play";
							}
						});
						if (btn != null)
							btn.Click += new EventHandler((s, ev) =>
							{
								if (playingButton == btn && playingSongSelection == cb.SelectedIndex)
								{
									StopSong();
									playingButton = null;
									btn.Text = "Play";
								}
								else
								{
									MusicID song;
									if (cb.SelectedIndex == 2) // Random
										song = optids[songPicker.Next(optids.Length)];
									else
										song = optids[cb.SelectedIndex - 3];
									PlaySong((byte)song);
									playingButton = btn;
									playingSongSelection = cb.SelectedIndex;
									btn.Text = "Stop";
								}
							});
					}
				}
				table.ResumeLayout();
			}
		}

		void SongStoppedInternal()
		{
			playingButton.Text = "Play";
			playingButton = null;
		}

		static void SongStopped()
		{
			Instance.Invoke(new Action(Instance.SongStoppedInternal));
		}

		private void setAllToDefaultToolStripMenuItem_Click(object sender, EventArgs e)
		{
			foreach (ComboBox cb in comboboxes[tabControl1.SelectedIndex])
				cb.SelectedIndex = 0;
		}

		private void setAllToMIDIToolStripMenuItem_Click(object sender, EventArgs e)
		{
			foreach (ComboBox cb in comboboxes[tabControl1.SelectedIndex])
				cb.SelectedIndex = 1;
		}

		private void setAllToRandomToolStripMenuItem_Click(object sender, EventArgs e)
		{
			foreach (ComboBox cb in comboboxes[tabControl1.SelectedIndex].Where(cb => cb.Items.Count > 2))
				cb.SelectedIndex = 2;
		}

		private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
		{
			foreach (string game in GameIDs)
			{
				Dictionary<string, string> group = INI[game];
				foreach (string track in TrackKeys)
					if (group[track].Equals("Default", StringComparison.OrdinalIgnoreCase))
						group.Remove(track);
				if (group.Count == 0)
					INI.Remove(game);
			}
			IniFile.IniFile.Save(INI, "SMPSOUT.ini");
			if (hModule != IntPtr.Zero)
				FreeLibrary(hModule);
		}
	}
}