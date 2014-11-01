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

		List<TrackInfoInternal> TrackOptions = new List<TrackInfoInternal>();

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
		unsafe delegate IntPtr *GetCustomSongsDelegate(out uint count);
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
			Dictionary<string, short> songNums = new Dictionary<string, short>(StringComparer.OrdinalIgnoreCase);
			short songCount = 0;
			foreach (string song in IniSerializer.Deserialize<SongList>(ini).songs.Keys)
				songNums.Add(song, songCount++);
			if (!File.Exists("opts.ini"))
			{
				MessageBox.Show(this, "Failed to load opts.ini.", Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
				Environment.Exit(-1);
			}
			ini = IniFileClass.Load("opts.ini");
			if (File.Exists("opts_SKC.ini"))
				ini = IniFileClass.Combine(ini, IniFileClass.Load("opts_SKC.ini"));
			foreach (KeyValuePair<string, TrackOptInfo> item in IniSerializer.Deserialize<Dictionary<string, TrackOptInfo>>(ini))
			{
				TrackInfoInternal track = new TrackInfoInternal(item.Key, item.Value.Name);
				if (item.Value.Options != null && item.Value.Options.Count > 0)
				{
					track.Options.Add(new TrackOptionInternal("Random"));
					foreach (KeyValuePair<string, string> opt in item.Value.Options)
						track.Options.Add(new TrackOptionInternal(opt.Key, songNums[opt.Value], true));
				}
				TrackOptions.Add(track);
			}
			if (File.Exists("SMPSOUT.ini"))
				INI = IniFile.IniFile.Load("SMPSOUT.ini");
			else
				INI = new Dictionary<string, Dictionary<string, string>>();
			IntPtr ptr;
			foreach (string file in Directory.GetFiles(Environment.CurrentDirectory, "*.dll"))
			{
				IntPtr handle = LoadLibrary(file);
				ptr = GetProcAddress(handle, "InitializeDriver");
				if (ptr == IntPtr.Zero)
				{
					FreeLibrary(handle);
					continue;
				}
				hModule = handle;
				((InitializeDriverDelegate)Marshal.GetDelegateForFunctionPointer(ptr, typeof(InitializeDriverDelegate)))();
				break;
			}
			if (hModule != IntPtr.Zero)
			{
				ptr = GetProcAddress(hModule, "PlaySong");
				PlaySong = (PlaySongDelegate)Marshal.GetDelegateForFunctionPointer(ptr, typeof(PlaySongDelegate));
				ptr = GetProcAddress(hModule, "StopSong");
				StopSong = (StopSongDelegate)Marshal.GetDelegateForFunctionPointer(ptr, typeof(StopSongDelegate));
				ptr = GetProcAddress(hModule, "RegisterSongStoppedCallback");
				((RegisterSongStoppedCallbackDelegate)Marshal.GetDelegateForFunctionPointer(ptr, typeof(RegisterSongStoppedCallbackDelegate)))(SongStoppedCallback);
				ptr = GetProcAddress(hModule, "GetCustomSongs");
				GetCustomSongs(songCount, ptr);
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
					table.Controls.Add(new Label() { Text = TrackOptions[tn].Name + ":", AutoSize = true, TextAlign = ContentAlignment.MiddleLeft }, 0, tn);
					ComboBox cb = new ComboBox() { DropDownStyle = ComboBoxStyle.DropDownList };
					List<TrackOptionInternal> opts = TrackOptions[tn].Options;
					foreach (TrackOptionInternal opt in opts)
						cb.Items.Add(opt.Name);
					table.Controls.Add(cb, 1, tn);
					comboboxes[gn].Add(cb);
					Button btn = hModule != IntPtr.Zero && opts.Any(a => a.TrackID.HasValue) ? new Button() { Enabled = false, Text = "Play" } : null;
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
						for (int i = 0; i < opts.Count; i++)
							if ((group[track] ?? "Default").Equals(opts[i].Name, StringComparison.OrdinalIgnoreCase))
							{
								cb.SelectedIndex = i;
								if (btn != null && (opts[i].TrackID.HasValue || opts[i].Name == "Random"))
									btn.Enabled = true;
								break;
							}
					if (cb.SelectedIndex == -1)
						cb.SelectedIndex = 0;
					if (tn > 0)
						table.RowStyles.Add(new RowStyle(SizeType.AutoSize));
					if (!opts.Any(a => a.TrackID.HasValue))
						cb.SelectedIndexChanged += new EventHandler((s, ev) => INI[game][track] = opts[cb.SelectedIndex].Name);
					else
					{
						cb.SelectedIndexChanged += new EventHandler((s, ev) =>
						{
							INI[game][track] = opts[cb.SelectedIndex].Name;
							if (btn != null)
							{
								btn.Enabled = opts[cb.SelectedIndex].TrackID.HasValue || opts[cb.SelectedIndex].Name == "Random";
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
									short song;
									if (opts[cb.SelectedIndex].TrackID.HasValue)
										song = opts[cb.SelectedIndex].TrackID.Value;
									else
									{
										List<short> songs = opts.Where(a => a.IncludeInRandom).Select(a => a.TrackID.Value).ToList();
										song = songs[songPicker.Next(songs.Count)];
									}
									PlaySong(song);
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

		unsafe private void GetCustomSongs(short songCount, IntPtr ptr)
		{
			uint custcnt;
			IntPtr *p = ((GetCustomSongsDelegate)Marshal.GetDelegateForFunctionPointer(ptr, typeof(GetCustomSongsDelegate)))(out custcnt);
			for (uint i = 0; i < custcnt; i++)
			{
				string str = Marshal.PtrToStringAnsi(*(p++));
				foreach (TrackInfoInternal item in TrackOptions)
					item.Options.Add(new TrackOptionInternal(str, songCount, false));
				++songCount;
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
			foreach (ComboBox cb in comboboxes[tabControl1.SelectedIndex].Where(cb => cb.Items.Contains("Random")))
				cb.SelectedIndex = cb.Items.IndexOf("Random");
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
		}
	}

	class TrackInfoInternal
	{
		public string Key { get; set; }
		public string Name { get; set; }
		public List<TrackOptionInternal> Options { get; set; }

		public TrackInfoInternal(string key, string name)
		{
			Key = key;
			Name = name;
			Options = new List<TrackOptionInternal>() { new TrackOptionInternal("Default"), new TrackOptionInternal("MIDI") };
		}
	}

	class TrackOptionInternal
	{
		public string Name { get; set; }
		public short? TrackID { get; set; }
		public bool IncludeInRandom { get; set; }

		public TrackOptionInternal(string name)
		{
			Name = name;
		}

		public TrackOptionInternal(string name, short trackID, bool includeInRandom)
			: this(name)
		{
			TrackID = trackID;
			IncludeInRandom = includeInRandom;
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