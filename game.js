function game( n ) {
	this.n = n;
	this.places = new Array( n );
	this.player = true;
	this.historyTable = [];

	self = this;

	$('#board').empty();
	this.updateVisual();

	for( var i =0; i < this.n; i++ ) {
		this.places[i] = $('<div class="place" >'+(i+1)+'</div>');
		this.places[i].set = false;
		$('#board').append(this.places[i]);
		this.places[i].click( {place: i} ,function(event) {
			self.doClick( event.data.place );
		});
	}

}

game.prototype.doClick = function doClick( i ) {
	var move = [];

	this.player = !this.player;
	this.setPlace(i);
	move.push(i);

	if( i > 0 && !this.places[i-1].set ) {
		this.setPlace(i-1);
		move.push(i-1);
	}
	if( i < this.n-1 && !this.places[i+1].set ) {
		this.setPlace(i+1);
		move.push(i+1);
	}

	this.historyTable.push(move);

	this.updateVisual();
}

game.prototype.setPlace = function setPlace(i) {
	this.places[i].css('background', '#AAA');
	this.places[i].unbind( "click" );
	this.places[i].set = true;
}

game.prototype.unsetPlace = function unsetPlace(i) {
	this.places[i].css('background', '#eee');
	self = this;
	this.places[i].click( {place: i} , function(event) {
		self.doClick( event.data.place );
	});
	this.places[i].set = false;
}

game.prototype.undo = function undo() {
	var move = this.historyTable.pop();
	if( typeof(move) != "undefined" ) {
		for( var i = 0; i < move.length; i++ )
			this.unsetPlace( move[i] );

		this.player = !this.player;
		this.updateVisual();
	}


}

game.prototype.updateVisual = function updateVisual( ) {
	var player = (this.player) ? "A" : "B";
	$('#whichPlayer').html('Player '+player);

	if( this.isDone() ) {
		$('#whichPlayer').html('Player '+player+' lost');
	}

}

game.prototype.isDone = function isDone() {
	for( var i=0; i < this.n; i++ ) {
		if( !this.places[i].set )
			return false;
	}
	return true;
}

var g;
$(document).ready(function() {
	$('#setN').click( function() {
		var n = $('#n').val();
		g = new game( n );

		$('#undo').unbind('click');
		$('#undo').bind( 'click', function() {
			g.undo();
		});
	});
});