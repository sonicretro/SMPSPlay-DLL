using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;
using IniFile;
using IniFileClass = IniFile.IniFile;

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

		static readonly string[] GameIDs = { string.Empty, "S3K", "S3", "S&K" };

		Dictionary<string, Dictionary<string, string>> INI;

		Dictionary<string, short> SongNums = new Dictionary<string, short>(StringComparer.OrdinalIgnoreCase);
		List<string> CustSongs = new List<string>();
		List<KeyValuePair<string, TrackOptInfo>> TrackOptions = new List<KeyValuePair<string, TrackOptInfo>>();

		static readonly List<ComboBox>[] comboboxes = new List<ComboBox>[4];
		static readonly List<Button> buttons = new List<Button>();

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
		delegate bool PlaySongDelegate(short song);
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
			if (!File.Exists("songs.ini"))
			{
				MessageBox.Show(this, "Failed to load songs.ini.", Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
				Environment.Exit(-1);
			}
			Dictionary<string, Dictionary<string, string>> ini = IniFileClass.Load("songs.ini");
			if (File.Exists("songs_SKC.ini"))
				ini = IniFileClass.Combine(ini, IniFileClass.Load("songs_SKC.ini"));
			short b = 0;
			foreach (string song in IniSerializer.Deserialize<SongList>(ini).songs.Keys)
				SongNums.Add(song, b++);
			if (File.Exists("songs_cust.ini"))
				foreach (string song in IniSerializer.Deserialize<SongList>("songs_cust.ini").songs.Where(s => File.Exists(s.Value.File)).Select(s => s.Key))
				{
					CustSongs.Add(song);
					SongNums.Add(song, b++);
				}
			if (!File.Exists("opts.ini"))
			{
				MessageBox.Show(this, "Failed to load opts.ini.", Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
				Environment.Exit(-1);
			}
			ini = IniFileClass.Load("opts.ini");
			if (File.Exists("opts_SKC.ini"))
				ini = IniFileClass.Combine(ini, IniFileClass.Load("opts_SKC.ini"));
			TrackOptions = IniSerializer.Deserialize<Dictionary<string, TrackOptInfo>>(ini).ToList();
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
				for (int tn = 0; tn < TrackOptions.Count; tn++)
				{
					string track = TrackOptions[tn].Key;
					table.Controls.Add(new Label() { Text = TrackOptions[tn].Value.Name + ":", AutoSize = true, TextAlign = ContentAlignment.MiddleLeft }, 0, tn);
					ComboBox cb = new ComboBox() { DropDownStyle = ComboBoxStyle.DropDownList };
					cb.Items.Add("Default");
					cb.Items.Add("MIDI");
					Dictionary<string, string> opts = TrackOptions[tn].Value.Options;
					string[] optnames = opts != null ? opts.Keys.ToArray() : null;
					string[] optids = opts != null ? opts.Values.ToArray() : null;
					if (opts.Count > 0)
					{
						cb.Items.Add("Random");
						cb.Items.AddRange(optnames);
						cb.Items.AddRange(CustSongs.ToArray());
					}
					table.Controls.Add(cb, 1, tn);
					comboboxes[gn].Add(cb);
					Button btn = hModule != IntPtr.Zero && (opts.Count > 0 || CustSongs.Count > 0) ? new Button() { Enabled = false, Text = "Play" } : null;
					if (btn != null)
					{
						table.Controls.Add(btn, 2, tn);
						buttons.Add(btn);
					}
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
								cb.SelectedIndex = opts.Count > 0 ? 2 : 0;
								if (btn != null)
									btn.Enabled = true;
								break;
							default:
								cb.SelectedIndex = 0;
								if (opts.Count > 0)
								{
									bool found = false;
									for (int on = 0; on < optnames.Length; on++)
										if (group[track].Equals(optnames[on], StringComparison.OrdinalIgnoreCase))
										{
											cb.SelectedIndex = on + 3;
											if (btn != null)
												btn.Enabled = true;
											found = true;
											break;
										}
									if (!found)
										for (int i = 0; i < CustSongs.Count; i++)
											if (group[track].Equals(CustSongs[i], StringComparison.OrdinalIgnoreCase))
											{
												cb.SelectedIndex = i + optnames.Length + 3;
												if (btn != null)
													btn.Enabled = true;
												break;
											}
								}
								else
									for (int i = 0; i < CustSongs.Count; i++)
										if (group[track].Equals(CustSongs[i], StringComparison.OrdinalIgnoreCase))
										{
											cb.SelectedIndex = i + 2;
											if (btn != null)
												btn.Enabled = true;
											break;
										}
								break;
						}
					if (tn > 0)
						table.RowStyles.Add(new RowStyle(SizeType.AutoSize));
					if (opts.Count == 0 && CustSongs.Count == 0)
						cb.SelectedIndexChanged += new EventHandler((s, ev) =>
						{
							string str;
							if (cb.SelectedIndex == 1)
								str = "MIDI";
							else
								str = "Default";
							INI[game][track] = str;
						});
					else if (opts.Count == 0)
					{
						cb.SelectedIndexChanged += new EventHandler((s, ev) =>
						{
							string str;
							if (cb.SelectedIndex > 1)
								str = CustSongs[cb.SelectedIndex - 2];
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
									string song = CustSongs[cb.SelectedIndex - 2];
									PlaySong(SongNums[song]);
									playingButton = btn;
									playingSongSelection = cb.SelectedIndex;
									foreach (Button item in buttons)
										item.Text = "Play";
									btn.Text = "Stop";
								}
							});
					}
					else
					{
						cb.SelectedIndexChanged += new EventHandler((s, ev) =>
						{
							string str;
							if (cb.SelectedIndex > optnames.Length + 2)
								str = CustSongs[cb.SelectedIndex - (optnames.Length + 3)];
							else if (cb.SelectedIndex > 2)
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
									string song;
									if (cb.SelectedIndex == 2) // Random
										song = optids[songPicker.Next(optids.Length)];
									else if (cb.SelectedIndex > optnames.Length + 2)
										song = CustSongs[cb.SelectedIndex - (optids.Length + 3)];
									else
										song = optids[cb.SelectedIndex - 3];
									PlaySong(SongNums[song]);
									playingButton = btn;
									playingSongSelection = cb.SelectedIndex;
									foreach (Button item in buttons)
										item.Text = "Play";
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
			foreach (ComboBox cb in comboboxes[tabControl1.SelectedIndex].Where(cb => cb.Items.Count > 2 && (string)cb.Items[2] == "Random"))
				cb.SelectedIndex = 2;
		}

		private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
		{
			foreach (string game in GameIDs)
			{
				Dictionary<string, string> group = INI[game];
				foreach (string track in TrackOptions.Select(a => a.Key))
					if (group[track].Equals("Default", StringComparison.OrdinalIgnoreCase))
						group.Remove(track);
				if (group.Count == 0)
					INI.Remove(game);
			}
			IniFileClass.Save(INI, "SMPSOUT.ini");
			if (hModule != IntPtr.Zero)
				FreeLibrary(hModule);
		}
	}

	class SongList
	{
		public int residstart { get; set; }
		[IniCollection(IniCollectionMode.IndexOnly)]
		public Dictionary<string, SongInfo> songs { get; set; }
	}

	class SongInfo
	{
		public string Type { get; set; }
		public string Offset { get; set; }
		public string File { get; set; }
	}

	class TrackOptInfo
	{
		public string Name { get; set; }
		[IniCollection(IniCollectionMode.IndexOnly)]
		public Dictionary<string, string> Options { get; set; }
	}
}