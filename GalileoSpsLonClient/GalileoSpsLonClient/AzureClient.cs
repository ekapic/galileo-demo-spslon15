using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using Microsoft.WindowsAzure.Storage;
using Microsoft.WindowsAzure.Storage.Auth;
using Microsoft.WindowsAzure.Storage.Table;
using GalileoSpsLonClient.Models;

namespace GalileoSpsLonClient
{
    public class AzureClient
    {
        private CloudTable roomTable;

        public AzureClient()
        {
            var storageCredentials = new StorageCredentials("galileospslon15", 
                "");
            CloudStorageAccount storageAccount = new CloudStorageAccount(storageCredentials, true);

            CloudTableClient client = storageAccount.CreateCloudTableClient();
            roomTable = client.GetTableReference("roomstatus");
        }

        public RoomAvailabilitySlot IsRoomFreeNow()
        {
            RoomAvailabilitySlot result = null;

            var now = DateTime.UtcNow;
            var timeToLookFor = new DateTime(now.Year, now.Month, now.Day, now.Hour, now.Minute, 0, DateTimeKind.Utc);
            var rowKey = timeToLookFor.AddMinutes(-1).ToString("yyyy'-'MM'-'dd'T'HH':'mm':00.0000000Z'");

            TableQuery<CloudRoomAvailability> query = new TableQuery<CloudRoomAvailability>()
                .Where(TableQuery.GenerateFilterCondition("RowKey", QueryComparisons.GreaterThanOrEqual, rowKey));

            var historyItems = roomTable.ExecuteQuery(query);
            var row = historyItems.Reverse().FirstOrDefault();

            if (row != null)
            {
                result = new RoomAvailabilitySlot()
                {
                    Time = row.windowend.ToShortTimeString(),
                    IsAvailable = row.roomstatus == 0.0
                };
            }

            return result;
        }

        public RoomAvailabilityHistory GetRoomHistory()
        {
            var result = new RoomAvailabilityHistory();
            result.Slots = new List<RoomAvailabilitySlot>();

            var now = DateTime.UtcNow;
            var timeToLookFor = new DateTime(now.Year, now.Month, now.Day, now.Hour, now.Minute, 0, DateTimeKind.Utc);
            
            var rowKey = timeToLookFor.AddMinutes(-10).ToString("yyyy'-'MM'-'dd'T'HH':'mm':00.0000000Z'");

            TableQuery<CloudRoomAvailability> query = new TableQuery<CloudRoomAvailability>()
                .Where(TableQuery.GenerateFilterCondition("RowKey", QueryComparisons.GreaterThanOrEqual, rowKey));

            var historyItems = roomTable.ExecuteQuery(query);

            foreach (var historyItem in historyItems.Reverse().Take(10))
            {
                result.Slots.Add(new RoomAvailabilitySlot()
                {
                    IsAvailable = historyItem.roomstatus == 0.0,
                    Time = historyItem.windowend.ToShortTimeString()
                });
            }

            return result;
        }
    }
}