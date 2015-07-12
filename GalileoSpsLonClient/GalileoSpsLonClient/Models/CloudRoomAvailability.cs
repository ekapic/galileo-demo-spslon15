using Microsoft.WindowsAzure.Storage.Table;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace GalileoSpsLonClient.Models
{
    public class CloudRoomAvailability: TableEntity
    {
        public string location { get; set; }
        public double roomstatus { get; set; }
        public DateTime windowend { get; set; }
    }
}