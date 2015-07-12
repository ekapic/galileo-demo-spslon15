window.roomStatus = window.roomStatus || (function () {
    var api = {};

    api.updateStatus = function () {
        $.getJSON('/api/RoomAvailability/', null)
            .success(function (data) {
                var $indicator = $("#room-status-indicator");
                if (!data) {
                    $indicator.attr("class", "roomstatus bg-warning");
                    $indicator.text("No live data for the room");
                }
                else {
                    if (data.IsAvailable) {
                        $indicator.attr("class", "roomstatus bg-success");
                        $indicator.text("Available (" + data.Time +")");
                    }
                    else {
                        $indicator.attr("class", "roomstatus bg-danger");
                        $indicator.text("Occupied (" + data.Time + ")");
                    }
                }
            });
        $.getJSON('/api/RoomAvailability/history', null)
            .success(function (data) {
                var tableHeader = "<table class='table'><thead><tr><th>Time</th><th>Status</th></tr></thead><tbody>";
                var tableRows = "";

                for (var i = 0; i < data.Slots.length; i++) {
                    var item = data.Slots[i];
                    var itemStatus = item.IsAvailable ? "Available" : "Occupied";
                    var cssClass = item.IsAvailable ? "success" : "danger";
                    tableRows = tableRows + "<tr class='" + cssClass + "'><td>" + item.Time + "</td><td>" + itemStatus + "</td></tr>";
                }

                var tableFooter = "</tbody></table>"
                

                $("#room-status-history").html(tableHeader + tableRows + tableFooter);
                
            });
    };
    return api;
})();