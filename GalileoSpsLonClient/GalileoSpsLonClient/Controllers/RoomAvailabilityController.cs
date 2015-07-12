using GalileoSpsLonClient.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Web.Http;

namespace GalileoSpsLonClient.Controllers
{
    public class RoomAvailabilityController : ApiController
    {
        [HttpGet]
        [Route("api/roomavailability")]
        public RoomAvailabilitySlot IsFreeNow()
        {
            var client = new AzureClient();
            var result = client.IsRoomFreeNow();
            return result;
        }

        [HttpGet]
        [Route("api/roomavailability/history")]
        public RoomAvailabilityHistory RoomHistory()
        {
            var client = new AzureClient();
            var result = client.GetRoomHistory();
            return result;
        }
    }
}
