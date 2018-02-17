{ stdenv, mk, pkgconfig, fetchgitLocal }:

stdenv.mkDerivation rec {
  name = "libtags";
  src = ./.;#fetchgitLocal ./.;

  buildInputs = [ mk ];
  propagatedBuildInputs = [ pkgconfig ];
  enableParallelBuilding = true;

  installPhase = ''
    cd src && mk -f mkfile.nix install && cd ..
    cd examples && mk -f mkfile.nix install INCLUDES=-I$out/include LIBS="-L$out/lib -ltags"
  '';

  meta = {
    description = "A cross-platform library for reading tags, designed for highly constrained environments";
    maintainers = with stdenv.lib.maintainers; [ ftrvxmtrx ];
    platforms = stdenv.lib.platforms.unix;
    license = stdenv.lib.licenses.mit;
  };
}
