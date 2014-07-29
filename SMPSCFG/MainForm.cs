using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace SMPSCFG
{
	public partial class MainForm : Form
	{
		public MainForm()
		{
			InitializeComponent();
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

		static readonly string[] TitleScreenTrackOptions = { "S3", "S&K", "S&K0525", "S3D" };

		static readonly string[] AngelIslandTrackOptions = { "S3K", "GreenGrove" };
		
		static readonly string[] HydrocityTrackOptions = { "S3K", "RustyRuin" };

		static readonly string[] CarnivalNightTrackOptions = { "MD", "PC", "SpringStadium" };

		static readonly string[] FlyingBatteryTrackOptions = { "S3K", "PanicPuppet" };

		static readonly string[] IceCapTrackOptions = { "MD", "PC", "DiamondDust" };

		static readonly string[] LaunchBaseTrackOptions = { "MD", "PC", "GeneGadget", "PanicPuppet" };

		static readonly string[] LavaReefTrackOptions = { "S3K", "VolcanoValley" };

		static readonly string[] MidbossTrackOptions = { "S3", "S&K", "S3D1", "S3D2" };

		static readonly string[] BossTrackOptions = { "S3K", "S3D1", "S3D2" };

		static readonly string[] DoomsdayTrackOptions = { "S3K", "S3D1", "S3D2", "FinalFight" };

		static readonly string[] SpecialStageTrackOptions = { "S3K", "S3D" };

		static readonly string[] KnucklesTrackOptions = { "S3", "S&K", "PC" };

		static readonly string[] OneUpTrackOptions = { "S3", "S&K" };

		static readonly string[] InvincibilityTrackOptions = { "S3", "S&K", "S3PC", "PC", "S3D" };

		static readonly string[] CompetitionMenuTrackOptions = { "MD", "PC", "S3D" };

		static readonly string[] LevelSelectTrackOptions = { "S3K", "S3D", "PanicPuppet1" };

		static readonly string[] AllClearTrackOptions = { "S3", "S&K", "S&K0525", "S3D" };

		static readonly string[] CreditsTrackOptions = { "S3", "S&K", "S3C", "S&K0525", "PC", "S3D" };

		static readonly string[] ContinueTrackOptions = { "S3", "S&K" };

		static readonly string[][] TrackOptions = {
			TitleScreenTrackOptions,
			AngelIslandTrackOptions,
			AngelIslandTrackOptions,
			HydrocityTrackOptions,
			HydrocityTrackOptions,
			null,
			null,
			CarnivalNightTrackOptions,
			CarnivalNightTrackOptions,
			FlyingBatteryTrackOptions,
			FlyingBatteryTrackOptions,
			IceCapTrackOptions,
			IceCapTrackOptions,
			LaunchBaseTrackOptions,
			LaunchBaseTrackOptions,
			AngelIslandTrackOptions,
			AngelIslandTrackOptions,
			null,
			null,
			LavaReefTrackOptions,
			LavaReefTrackOptions,
			null,
			LaunchBaseTrackOptions,
			LaunchBaseTrackOptions,
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
			DoomsdayTrackOptions,
			null,
			AllClearTrackOptions,
			CreditsTrackOptions,
			null
		};

		static readonly string[] GameIDs = { string.Empty, "S3K", "S3", "S&K" };

		Dictionary<string, Dictionary<string, string>> INI;

		static readonly List<ComboBox>[] comboboxes = new List<ComboBox>[4];

		private void MainForm_Load(object sender, EventArgs e)
		{
			TableLayoutPanel[] tables = { tableLayoutPanel1, tableLayoutPanel2, tableLayoutPanel3, tableLayoutPanel4 };
			if (File.Exists("SMPSOUT.ini"))
				INI = IniFile.IniFile.Load("SMPSOUT.ini");
			else
				INI = new Dictionary<string, Dictionary<string, string>>();
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
					string[] opts = TrackOptions[tn];
					if (opts != null)
					{
						cb.Items.Add("Random");
						cb.Items.AddRange(opts);
					}
					table.Controls.Add(cb, 1, tn);
					comboboxes[gn].Add(cb);
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
								break;
							default:
								cb.SelectedIndex = 0;
								if (opts != null)
									for (int on = 0; on < opts.Length; on++)
										if (group[track].Equals(opts[on], StringComparison.OrdinalIgnoreCase))
										{
											cb.SelectedIndex = on + 3;
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
						cb.SelectedIndexChanged += new EventHandler((s, ev) =>
						{
							string str;
							if (cb.SelectedIndex > 2)
								str = opts[cb.SelectedIndex - 3];
							else if (cb.SelectedIndex == 2)
								str = "Random";
							else if (cb.SelectedIndex == 1)
								str = "MIDI";
							else
								str = "Default";
							INI[game][track] = str;
						});
				}
				table.ResumeLayout();
			}
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
		}
	}
}