﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using x360ce.Engine.Data;

namespace x360ce.Engine
{
	public class SearchResult
	{
		public Setting[] Settings { get; set; }
		public Summary[] Summaries { get; set; }
		public PadSetting[] PadSettings { get; set; }
	}
}